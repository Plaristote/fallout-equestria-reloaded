import {getValueFromRange} from "behaviour/random.mjs";

export const RandomEncounterChart = {
  None:     0,
  Friendly: 1,
  Hostile:  2,
  Special:  3,
  Dungeon:  4
};

export class Game {
  constructor(model) {
  }

  outdoorsTick() {
    if (!game.worldmap.getCurrentCity()) {
      switch (this.randomEncounterCheck()) {
      case RandomEncounterChart.Friendly:
        this.triggerFriendlyEncounter();
        break ;
      case RandomEncounterChart.Hostile:
        this.triggerHostileEncounter();
        break ;
      case RandomEncounterChart.Special:
        console.log("/!!\\ SPECIAL ENCOUNTER (none implemented yet though)");
        break ;
      case RandomEncounterChart.Dungeon:
        console.log("/!!\\ FOUND A DUNGEON (none implemented yet though)");
        break ;
      }
    }
  }

  randomEncounterCheck() {
    const roll = getValueFromRange(0, 10000);

    console.log("encounter roll", roll);
    if (roll == 42)
      return RandomEncounterChart.Special;
    else if (roll >= 100 && roll <= 115)
      return RandomEncounterChart.Special;
    else if (roll >= 3200 && roll <= 3350)
      return RandomEncounterChart.Hostile;
    else if (roll >= 9900)
      return RandomEncounterChart.Friendly;
    return RandomEncounterChart.None;
  }

  triggerHostileEncounter() {
    const desertMaps = ["random-desert-1", "random-desert-2", "random-desert-cabin"];
    const desertEasyEncounters = [
      function(difficultyRoll) { return { "name": "Rats", "members": [{"sheet": "mutatedRat", "script": "rat.mjs", "amount": Math.min(3, Math.floor(4 * (difficultyRoll / 40)))}] }; },
      function(difficultyRoll) { return { "name": "Scorpions", "members": [{"sheet": "scorpion", "script": "scorpion.mjs", "amount": Math.min(3, Math.floor(4 * (difficultyRoll / 40)))}] }; }
    ];
    const candidateMaps       = desertMaps;
    const candidateEncounters = desertEasyEncounters;
    const mapRoll             = getValueFromRange(0, candidateMaps.length - 1);
    const encounterRoll       = getValueFromRange(0, candidateEncounters.length - 1);
    const difficultyRoll      = getValueFromRange(0, 100);

    console.log("encounterRoll", encounterRoll);
    const enemyParty1 = candidateEncounters[encounterRoll](difficultyRoll);
    enemyParty1.zone = "encounter-zone-1";
    console.log("encounterRoll", encounterRoll);
	  
    game.randomEncounters.prepareEncounter(candidateMaps[mapRoll], {
      "optional": (getValueFromRange(0, 100) + difficultyRoll / 2) <= game.player.statistics.outdoorsman,
      "title": "danger",
      "parties": [enemyParty1]
    });
  }

  triggerFriendlyEncounter() {
    const desertMaps = ["random-desert-cabin"];
    const candidateMaps       = desertMaps;
    const mapRoll             = getValueFromRange(0, candidateMaps.length - 1);

    game.randomEncounters.prepareEncounter(candidateMaps[mapRoll], {
      "optional": true,
      "title": i18n.t(`worldmap.places.${candidateMaps[mapRoll]}`)
    });
  }
}

export function create(model) {
  return new Game(model);
}
