import {Drink} from "./drink.mjs";
import {randomCheck} from "../behaviour/random.mjs";

class Alcohol extends Drink {
  consumedBy(target) {
    const drunk = target.getBuff("drunk");

    target.addBuff("drunk");
    if (drunk) {
      const script = drunk.getScriptObject();
      const maxCharge = script.hasTolerance ? 5 : 8;

      if (script.charges >= maxCharge) {
        randomCheck(target.statistics.poisonResistance, {
          failure:         this.passOut.bind(this, target, 120),
          criticalFailure: this.passOut.bind(this, target, 720)
        });
      }
    }
    else
      game.appendToConsole(i18n.t("messages.drunk"));
  }
  
  passOut(target, duration) {
    target.addBuff("ko");
    target.getBuff("ko").tasks.removeTask("trigger"); // KO will be manually removed later
    game.appendToConsole(i18n.t("messages.drunk-pass-out"));
    game.asyncAdvanceTime(duration, () => {
      target.getBuff("ko").remove();
    });
  }
}

export function create(model) {
  return new Alcohol(model);
}
