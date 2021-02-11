import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/ui"

Image {
  source: "qrc:/assets/backgrounds/default.jpg"
  anchors.fill: parent
  fillMode: Image.PreserveAspectCrop

  Action {
    id: continueAction
    text: qsTr("Continue")
    enabled: gameManager.hasContinueGame()
    onTriggered: application.gameLoading = gameManager.getSavedGames()[0]
  }

  Action {
    id: newGameAction
    text: qsTr("New game")
    onTriggered: application.createGame()
  }

  Action {
    id: loadGameAction
    text: qsTr("Load game")
    enabled: gameManager.getSavedGames().length > 0
    onTriggered: application.pushView("LoadGame.qml")
  }

  Action {
    id: exitAction
    text: qsTr("Exit")
    onTriggered: application.close();
  }

  Column {
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.margins: 50
    spacing: 10

    Repeater {
      model: [continueAction, newGameAction, loadGameAction, exitAction]
      delegate: MenuButton { action: modelData }
    }
  }
}
