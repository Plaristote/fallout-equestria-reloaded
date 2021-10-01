import {stealCheck} from "../cmap/helpers/checks.mjs";

export class ShopShelf {
  constructor(model) {
    this.model = model;
  }
  
  get shopScript() {
    return this.model.parent.getScriptObject();
  }

  get shopOwner() {
    return this.shopScript.shopOwner;
  }
  
  get shopOwnerScript() {
    return this.shopOwner.getScriptObject();
  }

  isUnderSurveillance() {
    return this.shopScript.isUnderSurveillance();
  }

  onUse() {
    const shopOwner = this.shopOwner;

    if (this.isUnderSurveillance())
    {
      level.addTextBubble(shopOwner, "Don't touch that.", 2500, "white");
      return true;
    }
    return false;
  }

  onTakeItem(user, item, quantity) {
    const shopOwner = this.shopOwner;

    if (this.isUnderSurveillance()) {
      return stealCheck(user, shopOwner, item, quantity, {
        failure:         this.onStealFailure.bind(this, user, false, item),
        criticalFailure: this.onStealFailure.bind(this, user, true, item)
      });
    }
    return true;
  }

  onPutItem(user, item, quantity) {
    return this.onTakeItem(user, item, quantity);
  }
  
  onStealFailure(user, critical, item) {
    this.shopScript.onShopliftAttempt(user);
  }
}

export function create(model) {
  return new ShopShelf(model);
}

