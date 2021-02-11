import QtQuick 2.15
import Game 1.0
import "./ui"

Rectangle {
  property QtObject gameController
  anchors.fill: parent
  color: "black"

  CharacterSheet {
    anchors.fill: parent
    characterSheet: gameController.getPlayerStatistics()

    Component.onCompleted: {
      characterSheet.specialPoints = 5;
    }

    onAccepted: {
      console.log("Start a new game");
      gameManager.currentGame.goToLevel("eltest5");
      application.popView();
    }
    onCanceled: {
      application.popView()
      gameManager.endGame();
    }
  }
}
