import {ShopOwner} from "./shop-owner.mjs";

class Baker extends ShopOwner {
  constructor(model) {
    super(model);
  }
}

export function create(model) {
  return new Baker(model);
}
