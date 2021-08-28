const tutorialByZone = {
  tutorialLockpick:     1,
  tutorialFindStuff:    6,
  tutorialDynamite:     7,
  tutorialFieldOfViews: 8,
  tutorialTraps:        9
};

export class Level {
  constructor(model) {
    model.tasks.addTask("onGameStarted", 1000, 1);
  }

  onGameStarted() {
    this.displayTutorialPage(0);
  }

  onZoneEntered(zoneName, object) {
    if (tutorialByZone[zoneName] != undefined)
      this.displayTutorialPage(tutorialByZone[zoneName]);
  }

  displayTutorialPage(page) {
    if (!level.hasVariable(`tutorial-${page}-shown`) && level.tutorial.enabled) {
      level.tutorial.page = page;
      level.tutorial.visible = true;
      level.setVariable(`tutorial-${page}-shown`, 1);
    }
  }

  onCombatStarted() {
    this.displayTutorialPage(3);
  }
  
  onCombatEnded() {
    if (!level.hasVariable("tutorialCombatEnd")) {
      level.player.statistics.addExperience(level.player.statistics.xpNextLevel);
      this.displayTutorialPage(5);
      level.setVariable("tutorialCombatEnd", 1);
    }
  }
}

export function create(model) {
  return new Level(model);
}
