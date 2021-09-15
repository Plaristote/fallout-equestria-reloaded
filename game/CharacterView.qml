import QtQuick 2.15
import "../ui"

CharacterSheet {
  property QtObject gameController

  characterSheet: gameController.getPlayerStatistics()
  mode: "view"

  Component.onCompleted: {
    swapPauseMode(true);
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

  function swapPauseMode(value) {
    if (gameController.level)
      gameController.level.paused = value;
    else
      gameController.worldmap.paused = value;
  }

  function close() {
    swapPauseMode(false);
    application.popView();
  }
}
