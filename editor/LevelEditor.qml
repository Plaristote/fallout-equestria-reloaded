import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "../game" as GameComponents
import Game 1.0

Item {
  property var gameController
  property alias currentLevelName: levelSelect.currentName

  onCurrentLevelNameChanged: {
    gameController.goToLevel(currentLevelName.replace(".json", ""));
    //gameController.level.paused = false;
  }

  RowLayout {
    anchors.fill: parent

    EditorSelectPanel {
      id: levelSelect
      model: scriptController.getLevels()
      readOnly: true
    }

    Pane {
      background: UiStyle.TerminalPane {}
      Layout.fillHeight: true
      Layout.fillWidth: true

      GameComponents.LevelCanvas {
        id: canvas
        levelController: gameController.level
        //anchors { top: parent.top; left: parent.left; bottom: parent.bottom; right: parent.right }
      }
    }
  }
}
