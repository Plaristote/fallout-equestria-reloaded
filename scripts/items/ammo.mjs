import {ItemBehaviour} from "./item.mjs";

class Ammo extends ItemBehaviour {
}

export function create(model) {
  return new Ammo(model);
}
