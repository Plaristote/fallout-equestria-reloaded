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
    enabled: application.hasSavedGame
    onTriggered: application.continueGame()
  }

  Action {
    id: newGameAction
    text: qsTr("New game")
    onTriggered: application.createGame()
  }

  Action {
    id: loadGameAction
    text: qsTr("Load game")
    onTriggered: application.loadGame()
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

    Repeater {
      model: [continueAction, newGameAction, loadGameAction, exitAction]
      delegate: MenuButton { action: modelData }
    }
  }
}
