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
  property bool     inCombat: levelController ? levelController.combat : false
  property var      appNames:      ["Clock", "Quests", "Archives", "Automap"]
  property var      appComponents: [waitApplication, questsApplication, archiveApplication]
  property string   currentApp:    "Quests"

  background: UiStyle.Pane {}

  Component.onCompleted: {
    pauseController.paused = true;
    appComponents = appComponents.concat(levelController ? [automapApplication] : [null]);
    contentApplication.sourceComponent = appComponents[appNames.indexOf(currentApp)];
  }

  function loadApplication() {
    const index = appNames.indexOf(currentApp);
    let application = appComponents[index];
    if (application == waitApplication && inCombat)
      application = waitApplicationDisabled;
    contentApplication.sourceComponent = application;
  }

  onCurrentAppChanged: loadApplication();
  onInCombatChanged: loadApplication();

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
        ColumnLayout {
          Repeater {
            model: appNames
            delegate: MenuButton {
              Layout.fillWidth: true
              text: i18n.t(`pipboy.${appNames[index]}`)
              textColor: appNames[index] === currentApp ? "yellow" : "white"
              onClicked: currentApp = appNames[index]
            }
          }
          MenuButton {
            Layout.fillWidth: true
            action: actions.close
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
      id: waitApplicationDisabled
      Item {
        Text {
          anchors.centerIn: parent
          text: i18n.t("pipboy.clock-disabled")
          color: "yellow"
          font.family: application.consoleFont.name
          font.pointSize: application.consoleFont.bigSize
        }
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
      Item {
        Text {
          anchors.centerIn: parent
          text: "Nothing here yet !"
          color: "yellow"
          font.family: application.consoleFont.name
          font.pointSize: application.consoleFont.bigSize
        }
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
