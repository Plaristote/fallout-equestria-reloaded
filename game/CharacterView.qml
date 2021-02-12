import QtQuick 2.15
import "../ui"

CharacterSheet {
  property QtObject gameController

  characterSheet: gameController.getPlayerStatistics()
  mode: "view"

  Component.onCompleted: {
    gameController.level.paused = true;
  }

  onCanceled: {
    gameController.level.paused = false;
    application.popView();
  }
}
