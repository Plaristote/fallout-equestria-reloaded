import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

Pane {
  id: mainMenu
  width: 280
  height: 250
  background: UiStyle.Pane {}

  Column {
    anchors.fill: parent
    anchors.topMargin: 10

    MenuButton {
      text: "Save Game"
      onClicked: application.pushView("SaveGame.qml")
    }

    MenuButton {
      text: "Load Game"
      onClicked: application.pushView("LoadGame.qml")
    }

    MenuButton {
      text: "Exit"
      onClicked: {
        console.log("exit");
        application.popView();
        gameManager.endGame();
      }
    }

    MenuButton {
      text: "Cancel"
      onClicked: mainMenu.visible = false
    }
  }
}
