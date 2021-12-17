import {randomCheck} from "./random.mjs";

function makeExplosionAnimationAt(x, y) {
  const ground = level.tilemap.getLayer("ground");
  const tile   = ground ? ground.getTile(x, y) : null;

  if (tile) {
    return {
      "type": "Sprite",
      "name": "effects",
      "animation": "explosion",
      "fromX": tile.renderPosition.x,
      "fromY": tile.renderPosition.y
    };
  }
  return {};
}

export class Explosion {
  constructor(position) {
    this.position = position;
    this.radius = 0;
    this.damage = 10;
    this.sound = "explosion";
  }

  withWearer(wearer) { this.wearer = wearer; return this; }
  withSound(sound)   { this.sound  = sound;  return this; }
  withDamage(damage) { this.damage = damage; return this; }
  withRadius(radius) { this.radius = radius; return this; }
  withDamageDealer(damageDealer) { this.damageDealer = damageDealer; return this; }

  trigger() {
    const fromPos  = [this.position.x - this.radius, this.position.y - this.radius];
    const toPos    = [this.position.x + this.radius, this.position.y + this.radius];

    for (var x = fromPos[0] ; x <= toPos[0] ; ++x) {
      for (var y = fromPos[1] ; y <= toPos[1] ; ++y) {
        const objects = level.getDynamicObjectsAt(x, y);

        level.addAnimationSequence({ steps: [makeExplosionAnimationAt(x, y)] });
        objects.forEach(this.triggeredOnObject.bind(this));
      }
    }
    game.sounds.play(this.sound);
  }

  triggeredOnObject(object) {
    switch (object.getObjectType()) {
    case "Character":
      this.applyDamageOnCharacter(object);
      break ;
    case "Doorway":
      this.applyDamageOnDoorway(object);
      break ;
    }
  }

  applyDamageOnCharacter(character) {
    var resistance = (character.statistics.strength * 2 + character.statistics.endurance + character.statistics.agility);
    var damage = this.damage;

    if (character === this.wearer)
      damage *= 2;
    damage -= character.statistics.damageResistance;
    damage = Math.max(0, damage);
    game.appendToConsole(i18n.t("messages.damaged", {
      target: character.statistics.name,
      damage: damage
    }));
    character.takeDamage(damage, this.damageDealer);
    console.log("Running resistance check for", character.statistics.name, "with threshold", resistance);
    randomCheck(resistance, {
      failure: this.onFailedResistCheck.bind(this, character, damage),
      criticalFailure: this.onCriticallyFailedResistCheck.bind(this, character, damage)
    });
  }

  onFailedResistCheck(character, damage) {
    const slideDistance = Math.max(1, damage / 10);

    console.log("- resistance check failed");
    character.fallAwayFrom(this.position.x, this.position.y, slideDistance);
  }

  onCriticallyFailedResistCheck(character, damage) {
    this.onFailedResistCheck(character, damage);
    console.log("TODO: implement critical failure to explosion resistance");
    character.addBuff("ko");
  }

  applyDamageOnDoorway(object) {
    if (object.destructible)
      object.bustOpen(this.damage);
  }
}


export function explode(position, radius, damage, wearer, damageDealer) {
  const explosion = new Explosion(position);

  explosion.withWearer(wearer)
           .withDamage(damage)
           .withRadius(radius)
           .withDamageDealer(damageDealer)
           .trigger();
}
