import {Shop} from "../shop.mjs";

class Library extends Shop {
  get shopOwner() {
    return this.model.findObject("librarian");
  }

  get guards() {
    return level.findGroup("guards");
  }
}

export function create(model) {
  return new Library(model);
}
