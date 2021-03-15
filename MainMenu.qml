import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/ui"

Image {
  source: "qrc:/assets/backgrounds/default.jpg"
  anchors.fill: parent
  fillMode: Image.PreserveAspectCrop

  Action {
    id: continueAction
    text: i18n.t("Continue")
    enabled: gameManager.hasContinueGame()
    onTriggered: application.gameLoading = gameManager.getSavedGames()[0]
  }

  Action {
    id: newGameAction
    text: i18n.t("New game")
    onTriggered: application.createGame()
  }

  Action {
    id: loadGameAction
    text: i18n.t("Load game")
    enabled: gameManager.getSavedGames().length > 0
    onTriggered: application.pushView("LoadGame.qml")
  }

  Action {
    id: optionsAction
    text: i18n.t("Options")
    onTriggered: application.pushView("Options.qml")
  }

  Action {
    id: exitAction
    text: i18n.t("Exit")
    onTriggered: application.close();
  }

  Action {
    id: gameEditorAction
    text: i18n.t("Game Editor")
    onTriggered: application.pushView("editor/Index.qml")
  }

  Column {
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.margins: 50
    spacing: 10

    Repeater {
      model: [continueAction, newGameAction, loadGameAction, optionsAction, gameEditorAction, exitAction]
      delegate: MenuButton { action: modelData }
    }
  }
}
