import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "./pipboy" as PipBoyUi

Pane {
  id: root
  property QtObject gameController:  gameManager.currentGame
  property QtObject levelController: gameController.level
  property var      appNames:      ["Clock", "Quests", "Archives", "Automap"]
  property var      appComponents: [waitApplication, questsApplication, archiveApplication]
  property string   currentApp:    "Quests"

  background: UiStyle.Pane {}

  Component.onCompleted: {
    pauseController.paused = true;
    appComponents = appComponents.concat(levelController ? [automapApplication] : [null]);
    contentApplication.sourceComponent = appComponents[appNames.indexOf(currentApp)];
  }

  onCurrentAppChanged: {
    const index = appNames.indexOf(currentApp);
    contentApplication.sourceComponent = appComponents[index];
  }

  PauseController { id: pauseController }

  PipBoyUi.Actions {
    id: actions
    pipboy: root
    onClosed: pauseController.paused = false
  }

  Connections {
    target: gameController.player
    function onDied() { application.popView(); }
  }

  RowLayout {
    anchors.fill: parent

    // Left pane
    ColumnLayout {
      Layout.preferredWidth: 400
      Layout.fillHeight: true

      ClockWidget {
        Layout.alignment: Qt.AlignTop | Qt.AlignCenter
      }

      Item {
        Layout.fillHeight: true
      }

      Pane {
        background: UiStyle.Pane {}
        Layout.alignment: Qt.AlignBottom | Qt.AlignCenter
        Column {
          Repeater {
            model: appNames
            delegate: MenuButton {
              text: i18n.t(`pipboy.${appNames[index]}`)
              textColor: appNames[index] === currentApp ? "yellow" : "white"
              onClicked: currentApp = appNames[index]
            }
          }

          MenuButton { action: actions.close }
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

    Component {
      id: archiveApplication
      Text {
        text: "Nothing here yet !"
      }
    }

    Component {
      id: automapApplication
      PipBoyUi.AutomapApplication {
        levelController: root.levelController
      }
    }
  }
}
