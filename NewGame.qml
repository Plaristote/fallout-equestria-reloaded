import QtQuick 2.15
import Game 1.0
import "./ui"

Rectangle {
  anchors.fill: parent
  color: "black"

  CharacterSheet {
    anchors.fill: parent
    characterSheet: StatModel {
      specialPoints: 5
      Component.onCompleted: specialChanged()
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
