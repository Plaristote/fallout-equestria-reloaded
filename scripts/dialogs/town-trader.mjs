class ButterQuestDialog {
  constructor(dialog) {
    this.dialog = dialog;
  }
	
  butterQuestAccepted() {
    this.dialog.npc.setVariable("butterQuest", 1);
    game.quests.addQuest("sampleTownButter");
  }
  
  hasCompletedButterQuest() {
    if (this.dialog.npc.path === "bakery.owner" && game.quests.hasQuest("sampleTownButter"))
      return game.player.inventory.count("food-butter") > 0;
    return false;
  }

  completeButterQuest() {
    const quest = game.quests.getQuest("sampleTownButter");

    game.player.inventory.removeItemOfType("food-butter");
    this.shopShelfs[0].inventory.addItemOfType("food-croissant", 24);
    quest.completeObjective("give");
    quest.completed = true;
  }
  
  croissantWait() {
    const state = this.dialog.npc.hasVariable("croissantWait") ? this.dialog.npc.getVariable("croissantWait") : 0;
    
    if (state === 0)
      level.moveCharacterToZone(this.dialog.npc, level.findGroup("bakery.backroom").controlZone);
    this.dialog.npc.setVariable("croissantWait", state + 1);
    game.advanceTime(30);
    return this.dialog.t(`wait-step-${state}`);
  }

  croissantWaitContinue() {
    const state = this.dialog.npc.hasVariable("croissantWait") ? this.dialog.npc.getVariable("croissantWait") : 0;

    return this.dialog.t(`wait-step-${state}-reaction`);
  }

  onCroissantWaitContinue() {
    const state = this.dialog.npc.hasVariable("croissantWait") ? this.dialog.npc.getVariable("croissantWait") : 0;

    if (state === 3) {
      level.setCharacterPosition(this.dialog.npc, 36, 51);
      game.player.inventory.addItemOfType("food-croissant", 2);
      game.quests.getQuest("sampleTownButter").completeObjective("reward");
      return "butter-quest-done";
    }
    return "butter-quest-wait";
  }
}

class Dialog extends ButterQuestDialog {
  constructor(dialog) {
    super(dialog);
    console.log("-- Town trader dialog building --");
    this.dialog.barter.removeInventory(this.dialog.npc.inventory);
    for (var i = 0 ; i < this.shopShelfs.length ; ++i) {
      console.log("-> adding shelf", this.shopShelfs[i]);
      this.dialog.barter.addInventory(this.shopShelfs[i].objectName, this.shopShelfs[i].inventory);
    }
  }
  
  get shop() {
    const container = this.dialog.npc.parent.findGroup("shop");

    return container ? container : this.dialog.npc.parent;
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
    if (this.dialog.npc.path === "bakery.owner" && !this.dialog.npc.hasVariable("butterQuest"))
      this.dialog.loadState("butter-quest-hook");
    else
      this.dialog.loadState("after-trade");
  }
}

export function create(dialog) {
  return new Dialog(dialog);
}
