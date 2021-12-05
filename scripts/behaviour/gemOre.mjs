class GemOre {
  constructor(model) {
    this.model = model;
  }

  getAvailableInteractions() {
    return ["look", "use-object", "use-skill"];
  }

  
  onUseItem(user, item) {
    if (item.itemType === "pickaxe")
      this.onMine(user);
  }

  onMine(user) {
    const quantity = Math.ceil(Math.random() * 5);
    var mineCount = 0;

    if (this.model.hasVariable("mineCount"))
      mineCount = this.model.getVariable("mineCount");
    if (mineCount < 3)
    {
      game.appendToConsole(i18n.t("messages.collected-ore", { quantity: quantity }));
      user.inventory.addItemOfType("gemstone", quantity);
      this.model.setVariable("mineCount", mineCount + 1);
    }
    else
      game.appendToConsole(i18n.t("messages.collected-ore-failure"));
  }
}

export function create(model) {
  return new GemOre(model);
}
