import QtQuick 2.15
import "../ui"

CharacterSheet {
  property QtObject gameController

  characterSheet: gameController.getPlayerStatistics()
  mode: "view"

  Component.onCompleted: {
    gameController.level.paused = true;
    characterSheet.hasLeveledUp = false;
    if (characterSheet.skillPoints > 0)
      mode = "edit";
  }

  onAccepted: {
    characterSheet.confirmChanges();
    close();
  }

  onCanceled: {
    characterSheet.cancelChanges();
    close();
  }

  function close() {
    gameController.level.paused = false;
    application.popView();
  }
}
