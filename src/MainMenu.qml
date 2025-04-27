import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
import "qrc:/ui"

Image {
  property var menuEntries: [continueAction, newGameAction, loadGameAction, optionsAction, creditsAction, exitAction]
  source: assetPath + "backgrounds/default.png"
  fillMode: Image.PreserveAspectCrop

  Component.onCompleted: {
    if (developmentEdition)
      menuEntries = menuEntries.slice(0, 4).concat([gameEditorAction, creditsAction, exitAction]);
  }

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
    id: creditsAction
    text: i18n.t("Credits")
    onTriggered: application.pushView("Credits.qml")
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

  ButtonNavigation {
    buttonRepeater: repeater
    currentIndex: -1
  }

  ColumnLayout {
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.margins: 50
    spacing: 10

    Repeater {
      id: repeater
      model: menuEntries
      delegate: MenuButton {
        Layout.fillWidth: true
        action: modelData
      }
    }
  }
}
