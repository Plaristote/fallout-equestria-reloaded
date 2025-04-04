import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
import "qrc:/assets/ui" as UiStyle
import "../../ui"

Pane {
  id: mainMenu
  property bool activated: false
  background: UiStyle.Pane {}
  onActivatedChanged: visible = activated
  visible: activated

  ColumnLayout {
    id: entries
    anchors.fill: parent
    anchors.topMargin: 10

    MenuButton {
      Layout.fillWidth: true
      text: i18n.t("Save game")
      enabled: !gameManager.currentGame.saveLock
      onClicked: application.pushView("SaveGame.qml")
    }

    MenuButton {
      Layout.fillWidth: true
      text: i18n.t("Load game")
      onClicked: application.pushView("LoadGame.qml")
    }

    MenuButton {
      Layout.fillWidth: true
      text: i18n.t("Options")
      onClicked: application.pushView("Options.qml")
    }

    MenuButton {
      Layout.fillWidth: true
      text: i18n.t("Exit")
      onClicked: {
        application.popView();
        gameManager.endGame();
      }
    }

    MenuButton {
      Layout.fillWidth: true
      text: i18n.t("Cancel")
      onClicked: mainMenu.visible = false
    }
  }
}
