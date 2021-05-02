import {ItemBehaviour} from "./item.mjs";

export class ArmorBehaviour extends ItemBehaviour {
  constructor(model) {
    super(model);
    this.triggersCombat = false;
    this.armorClass = 0;
  }
  
  canEquipInSlotType(slotType) {
    return slotType == "armor";
  }

  onEquipped(user, on) {
    if (on)
      user.statistics.armorClass += this.armorClass;
    else
      user.statistics.armorClass -= this.armorClass;
  }
}

export function create(model) {
  return new ArmorBehaviour(model);
}
