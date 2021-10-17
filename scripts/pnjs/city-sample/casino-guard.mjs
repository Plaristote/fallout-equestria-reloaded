import {GuardBehaviour} from "../guard.mjs";
import {GuardComponent} from "../components/guard.mjs";

class CasinoGuard extends GuardBehaviour {
  constructor(model) {
    super(model);
    this.guardComponent = new GuardComponent(this);
  }
}

export function create(model) {
  return new CasinoGuard(model);
}