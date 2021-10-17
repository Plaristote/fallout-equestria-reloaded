import {getValueFromRange} from "../behaviour/random.mjs";
import {skillContest} from "../cmap/helpers/checks.mjs";
import {DialogHelper} from "./helpers.mjs";

class Dialog extends DialogHelper {
  constructor(dialog) {
    super(dialog);
  }

  getEntryPoint() {
    if (this.dialog.npc.hasVariable("workInProgress"))
      return "introWithWork";
    return "intro";
  }

  intro() {
    if (this.dialog.npc.hasVariable("proposedWork"))
      return i18n.t("dialogs.tutoDog.introAlt");
    return i18n.t("dialogs.tutoDog.intro");
  }

  onAboutToExit() {
    if (!this.dialog.npc.hasVariable("proposedWork"))
    {
      this.dialog.npc.setVariable("proposedWork", 1);
      return "workProposal";
    }
  }

  canNegotiate() {
    return this.dialog.player.statistics.speech > 50;
  }

  tryToNegotiate() {
    const winner = skillContest(this.dialog.player, this.dialog.npc, "speech");

    if (winner === this.dialog.player);
      return "workAccepted";
    return "";
  }

  onWorkAccepted() {
    this.dialog.npc.setVariable("workInProgress", 1);
    game.quests.addQuest("fetchGemstones");
    this.dialog.npc.inventory.removeItemOfType("pickaxe");
    this.dialog.player.inventory.addItemOfType("pickaxe");
    this.dialog.mood = "smile";
  }

  workUncomplete() {
    this.dialog.npc = "angry";
  }

  hasGemstones() {
    return this.dialog.player.inventory.count("gemstone") > 0;
  }

  onWorkComplete() {
    const quest = game.quests.getQuest("fetchGemstones");
    const gemstoneCount = this.dialog.player.inventory.count("gemstone");

    this.dialog.mood = "smile";
    this.dialog.npc.unsetVariable("workInProgress");
    this.dialog.npc.setVariable("questDone", 1);
    this.dialog.npc.inventory.addItemOfType("gemstone", gemstoneCount);
    this.dialog.player.inventory.removeItemOfType("gemstone", gemstoneCount);
    quest.completeObjective("give-gemstones");
    quest.completed = true;
  }
}

export function create(dialog) {
  return new Dialog(dialog);
}