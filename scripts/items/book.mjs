import {Item} from "./item.mjs";
import {increaseBookCount} from "../cmap/perks/bookworm.mjs";

class Book extends Item {
  constructor(model) {
    super(model);
    this.useModes = ["use"];
    this.disabledCombatUse = true;
  }

  get requiresTarget() {
    return !this.user || this.user.inventory.isEquippedItem(this.model);
  }

  isGroupable(other) {
    if (other) {
      return other.itemType === this.model.itemType
          && other.hasVariable("read") === this.model.hasVariable("read");
    }
    return false;
  }

  isValidTarget(object) {
    return object === game.player;
  }

  useOn(target) {
    if (!target)
      target = this.user;
    if (target === game.player) {
      this.onRead(target);
      return true;
    }
    else
      game.appendToConsole(i18n.t("messages.invalid-target"));
    return false;
  }

  onRead(character) {
    const alreadyRead = this.model.hasVariable("read");
    const time = Math.floor(60 + (Math.random() * (alreadyRead ? 600 : 6)));

    game.asyncAdvanceTime(time);
    if (!alreadyRead) {
      this.setAsRead();      
      if (time >= 300)
        game.appendToConsole(i18n.t("messages.read-long-book"));
      else
        game.appendToConsole(i18n.t("messages.read-book"));
      increaseBookCount(character);
    }
    else {
      game.appendToConsole(i18n.t("messages.read-old-book"));
    }
  }
  
  setAsRead() {
    const readStack = this.getReadItemStack();

    if (readStack) {
      readStack.quantity++;
      if (this.model.quantity > 1)
        this.model.quantity--;
      else
        this.user.inventory.destroyItem(this.model);
    }
    else {
      this.model.setVariable("read", true);
      if (this.model.quantity > 1) {
        const remainings = this.model.quantity - 1;
        console.log("removing book", remainings);
        this.model.remove(remainings);
        this.user.inventory.addItemOfType(this.model.itemType, remainings);
      }        
    }
  }
  
  getReadItemStack() {
    for (var i = 0 ; i < this.user.inventory.items.length ; ++i) {
      const item = this.user.inventory.items[i];

      if (item.itemType === this.model.itemType && item.hasVariable("read"))
        return item;
    }
    return null;
  }
}

export function create(model) {
  return new Book(model);
}
