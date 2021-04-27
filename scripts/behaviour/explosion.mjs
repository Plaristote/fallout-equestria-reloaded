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

export function explode(position, radius, damage, wearer, damageDealer) {
  const fromPos  = [position.x - radius, position.y - radius];
  const toPos    = [position.x + radius, position.y + radius];

  for (var x = fromPos[0] ; x <= toPos[0] ; ++x) {
    for (var y = fromPos[1] ; y <= toPos[1] ; ++y) {
      const objects = level.getDynamicObjectsAt(x, y);

      level.addAnimationSequence({ steps: [makeExplosionAnimationAt(x, y)] });
      objects.forEach(object => {
        console.log("-> object in explode zone", object.objectName, object.getObjectType());
        switch (object.getObjectType()) {
          case "Character":
            object.takeDamage(object == wearer ? damage * 2 : damage, damageDealer);
            break ;
          case "Doorway":
            console.log("Doorway", object.destructible);
            if (object.destructible)
              object.bustOpen(damage);
            break ;
        }
      });
    }
  }
  level.sounds.play("explosion");
}
