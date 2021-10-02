class Dialog {
  constructor(dialog) {
    this.dialog = dialog;
    console.log("-- Town trader dialog building --");
    this.dialog.barter.removeInventory(this.dialog.npc.inventory);
    for (var i = 0 ; i < this.shopShelfs.length ; ++i) {
      console.log("-> adding shelf", this.shopShelfs[i]);
      this.dialog.barter.addInventory(this.shopShelfs[i].objectName, this.shopShelfs[i].inventory);
    }
  }
  
  get shop() {
    const container = this.dialog.npc.parent;

    if (container.name === "shop")
      return container;
    return container.findGroup("shop");
  }
  
  get shopShelfs() {
    const shop = this.shop;
    const result = [];
    
    for (var i = 0 ; i < shop.objects.length ; ++i) {
      if (shop.objects[i].getObjectType() === "StorageObject")
        result.push(shop.objects[i]);
    }
    return result;
  }
  
  startBarter() {
    this.dialog.tryToBarter();
    return this.dialog.t("trade-show");
  }
  
  onBarterEnded() {
    this.dialog.loadState("after-trade");
  }
}

export function create(dialog) {
  return new Dialog(dialog);
}
