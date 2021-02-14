import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import Game 1.0

Pane {
  id: root
  background: UiStyle.Pane {}

  Controller {
    id: game
  }

  TabRow {
    id: header
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    tabs: ["sprites", "objects", "npcs", "levels", "worldmap"]
    labels: ["Sprites", "Objects", "NPCs", "Levels", "Worldmap"]
    currentTab: tabs[0]
  }

  MenuButton {
    anchors.right: parent.right
    anchors.top: parent.top
    text: "Exit"
    onClicked: application.popView()
  }

  SpriteEditor {
    visible: header.currentTab == "sprites"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
  }

  ObjectEditor {
    visible: header.currentTab == "objects"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
  }

  NpcEditor {
    visible: header.currentTab == "npcs"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
  }

  LevelEditor {
    visible: header.currentTab == "levels"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
    gameController: game
  }
}
