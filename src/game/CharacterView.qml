import QtQuick 2.15
import "../ui"

CharacterSheet {
  property QtObject gameController

  characterSheet: gameController.getPlayerStatistics()
  mode: "view"

  Component.onCompleted: {
    pauseController.paused = true;
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
    pauseController.paused = false;
    application.popView();
  }

  PauseController { id: pauseController }
}
