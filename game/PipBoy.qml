import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "./pipboy" as PipBoyUi

Pane {
  id: root
  property QtObject gameController
  property QtObject levelController

  background: UiStyle.Pane {}

  Component.onCompleted: {
    levelController.paused = true;
    contentApplication.sourceComponent = questsApplication
  }

  Connections {
    target: gameController.player

    function onDied() {
      application.popView();
    }
  }

  RowLayout {
    anchors.fill: parent

    // Left pane
    ColumnLayout {
      Layout.preferredWidth: 400
      Layout.fillHeight: true

      PipBoyUi.TimeDisplay {
        timeManager: gameController.timeManager
        Layout.alignment: Qt.AlignTop | Qt.AlignCenter
        onWaitClicked: {
          contentApplication.sourceComponent = waitApplication
        }
      }

      Item {
        Layout.fillHeight: true
      }

      Pane {
        background: UiStyle.Pane {}
        Layout.alignment: Qt.AlignBottom | Qt.AlignCenter
        Column {
          MenuButton {
            text: "Quests"
            onClicked: contentApplication.sourceComponent = questsApplication
          }
          MenuButton {
            text: "Archives"
          }
          MenuButton {
            text: "Automap"
          }
          MenuButton {
            text: "Close"
            onClicked: {
              levelController.paused = false;
              application.popView();
            }
          }
        }
      }
    }

    // Content Pane
    Pane {
      background: UiStyle.TerminalPane {}
      Layout.fillWidth: true
      Layout.fillHeight: true

      Loader {
        id: contentApplication
        anchors.fill: parent
        anchors.margins: 10
        clip: true
      }
    }

    // Pages
    Component {
      id: waitApplication
      PipBoyUi.TimeApplication {
        gameController: root.gameController
      }
    }

    Component {
      id: questsApplication
      PipBoyUi.QuestsApplication {
        questManager: root.gameController.getQuestManager()
      }
    }
  }
}
