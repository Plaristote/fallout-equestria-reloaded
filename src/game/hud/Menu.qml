import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
import "qrc:/assets/ui" as UiStyle
import "../../ui"

Rectangle {
  property bool activated: false
  property var actions: [saveGameAction, loadGameAction, optionsAction, exitAction, cancelAction]
  id: mainMenu
  color: Qt.rgba(0, 0, 0, 0.5)
  onActivatedChanged: {
    visible = activated
    buttonNavigation.currentIndex = -1
  }
  visible: activated

  ButtonNavigation {
    id: buttonNavigation
    buttonRepeater: menuRepeater
    enabled: mainMenu.activated
  }

  Action {
    id: saveGameAction
    text: i18n.t("Save game")
    enabled: !gameManager.currentGame.saveLock
    onTriggered: application.pushView("SaveGame.qml")
  }

  Action {
    id: loadGameAction
    text: i18n.t("Load game")
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
    onTriggered: {
      application.popView();
      gameManager.endGame();
    }
  }

  Action {
    id: cancelAction
    text: i18n.t("Cancel")
    onTriggered: mainMenu.activated = false
  }

  MouseArea {
    anchors.fill: parent
  }

  Pane {
    anchors.centerIn: parent
    background: UiStyle.Pane {}
    implicitHeight: entries.height + 2 * background.borderSize
    implicitWidth: entries.width + 2 * background.borderSize

    ColumnLayout {
      id: entries
      anchors.centerIn: parent

      Repeater {
        id: menuRepeater
        model: actions
        delegate: MenuButton {
          Layout.fillWidth: true
          action: mainMenu.actions[index]
        }
      }
    }
  }
}
