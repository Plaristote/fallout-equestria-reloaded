import {Item} from "./item.mjs";
import {getValueFromRange} from "../behaviour/random.mjs";

class Dynamite extends Item {
  constructor(model) {
    super(model);
  }

  getActionPointCost() {
    return 5;
  }

  useOn(target) {
    this.trigger(5000);
    game.appendToConsole("ca va peter");
  }

  trigger(timeout) {
    this.model.tasks.addTask("explode", timeout, 1);
  }

  explode() {
    const wearer   = this.model.getOwner();
    const radius   = 3;
    const position = wearer ? wearer.position : this.model.position;
    const fromPos  = [position.x - radius, position.y - radius];
    const toPos    = [position.x + radius, position.y + radius];
    const damage   = getValueFromRange(20, 50);

    console.log("Exploding for", damage, "damage.");
    for (var x = fromPos[0] ; x <= toPos[0] ; ++x) {
      for (var y = fromPos[1] ; y <= toPos[1] ; ++y) {
        const objects = level.getDynamicObjectsAt(x, y);

        objects.forEach(object => {
          console.log("-> object in explode zone", object.objectName, object.getObjectType());
          switch (object.getObjectType()) {
            case "Character":
              object.takeDamage(object == wearer ? damage * 2 : damage, null);
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
    if (wearer)
      wearer.inventory.destroyItem(this.model, 1);
    level.deleteObject(this.model);
  }
}

export function create(model) {
  return new Dynamite(model);
}
