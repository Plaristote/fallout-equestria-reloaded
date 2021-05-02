import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

Pane {
  id: mainMenu
  width: 280
  height: 300
  background: UiStyle.Pane {}

  Column {
    anchors.fill: parent
    anchors.topMargin: 10

    MenuButton {
      text: i18n.t("Save game")
      onClicked: application.pushView("SaveGame.qml")
    }

    MenuButton {
      text: i18n.t("Load game")
      onClicked: application.pushView("LoadGame.qml")
    }

    MenuButton {
      text: i18n.t("Options")
      onClicked: application.pushView("Options.qml")
    }

    MenuButton {
      text: i18n.t("Exit")
      onClicked: {
        application.popView();
        gameManager.endGame();
      }
    }

    MenuButton {
      text: i18n.t("Cancel")
      onClicked: mainMenu.visible = false
    }
  }
}
