class Jail {
  constructor(model) {
    this.model = model;
  }

  get guard() {
    return this.model.findObject("guard");
  }

  get wantedVarname() {
    return level.name + this.model.path + '.wanted';
  }

  sendToRandomCell(character) {
    const cell = Math.floor(Math.random() * 101) % Math.max(1, this.model.groups.length);

    this.sendToCell(character, cell);
  }

  sendToCell(character, cellNumber) {
    const cell = this.model.groups[cellNumber];

    if (cell) {
      const door = cell.findObject("door");

      door.opened = false;
      door.locked = true;
      level.moveCharacterToZone(character, cell.controlZone);
    }
  }

  sentencePlayer(duration) {
    level.player.setVariable(this.wantedVarname, 1);
    this.model.tasks.removeTask("playerSentenceEnded");
    this.model.tasks.addTask("playerSentenceEnded", duration, 1);
  }

  playerSentenceEnded() {
    level.player.unsetVariable(this.wantedVarname);
    for (var i = 0 ; i < this.model.groups.length ; ++i) {
      const cell = this.model.groups[i];

      if (cell.getControlZoneOccupants().indexOf(level.player)) {
        level.player.unsetVariable(this.wantedVarname);
        level.moveCharacterToZone(level.player, this.model.controlZone);
        game.appendToConsole(i18n.t("messages.jail-sentence-over"));
        break ;
      }
    }
  }

  isPlayerWanted() {
    return level.player.hasVariable(this.wantedVarname);
  }

  shouldPlayerBeInJail() {
    return this.model.tasks.hasTask("playerSentenceEnded");
  }
}

export class JailComponent {
  constructor(parent, path) {
    this.model = parent.model;
    this.jailPath = path;
  }

  get jail() {
    return level.findGroup(this.jailPath).getScriptObject();
  }

  onCharacterDetected(character) {
    if (character === level.player && this.jail) {
      if (this.jail.isPlayerWanted()) {
        level.initializeDialog(this.model, "town-guard");
        return true;
      }
    }
    return false;
  }
}

export function create(model) {
  return new Jail(model);
}