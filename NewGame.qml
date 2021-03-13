import QtQuick 2.15
import Game 1.0
import "./ui"

Rectangle {
  property QtObject gameController
  property QtObject statModel: gameController.getPlayerStatistics()
  anchors.fill: parent
  color: "black"

  CharacterSheet {
    anchors.fill: parent
    characterSheet: statModel

    Component.onCompleted: {
      characterSheet.hitPoints = characterSheet.maxHitPoints
      characterSheet.specialPoints = 5;
    }

    onAccepted: {
      console.log("Start a new game");
      statModel.faction = "player";
      gameManager.currentGame.onCityEntered("eltest5");
      application.popView();
    }
    onCanceled: {
      application.popView()
      gameManager.endGame();
    }
  }

  Connections {
    target: statModel
    function onStatisticsChanged() {
      statModel.hitPoints = statModel.maxHitPoints
    }
  }
}
