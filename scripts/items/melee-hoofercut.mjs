import {MeleeAttack} from "./melee.mjs";

class Hoofercut extends MeleeAttack {
};

export function create(model) {
  return new Hoofercut(model);
}
