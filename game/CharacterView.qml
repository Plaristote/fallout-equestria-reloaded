import QtQuick 2.15
import "../ui"

CharacterSheet {
  property QtObject gameController

  characterSheet: gameController.getPlayerStatistics()
  mode: "view"

  Component.onCompleted: {
    gameController.currentLevel.paused = true;
  }


  onCanceled: {
    gameController.currentLevel.paused = false;
    application.popView();
  }
}
