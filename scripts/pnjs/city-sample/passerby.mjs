import {PasserbyBehaviour} from "../passerby.mjs";

const locations = [
  "library", "mansion.shop", {x:23,y:39}, {x:17,y:61}, {x:40,y:48}, {x:68,y:49}
];

class Passerby extends PasserbyBehaviour {
  constructor(model) { super(model, locations); }
}

export function create(model) {
  return new Passerby(model);
}
