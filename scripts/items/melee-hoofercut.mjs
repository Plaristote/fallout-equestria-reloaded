import {MeleeAttack} from "./melee.mjs";

class Hoofercut extends MeleeAttack {
};

export function create() {
  return new Hoofercut();
}
