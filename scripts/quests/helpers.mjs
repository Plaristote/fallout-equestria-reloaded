export class QuestHelper {
  constructor(model) {
    this.model = model;
  }

  tr(name, options = {}) {
    return i18n.t(`quests.${this.model.name}.${name}`, options);
  }

  completeObjective(objective) {
    this.model.setVariable(objective, 1);
  }

  isObjectiveCompleted(objective) {
    return this.model.getVariable(objective) === 1;
  }
}

