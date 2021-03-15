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
    isGameEditor: true
    Component.onCompleted: prepareEditor()
  }

  TabRow {
    id: header
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    tabs: ["worldmap", "levels", "npcs", "dialogs", "diplomacy", "objects", "sprites"]
    labels: ["Worldmap", "Levels", "NPCs", "Dialogs", "Diplomacy", "Items", "Sprites"]
    currentTab: tabs[0]
  }

  Button {
    background: UiStyle.Label { style: parent.down ? "dark" : "base" }
    anchors.right: parent.right
    anchors.top: parent.top
    contentItem: Text {
      color: "white"
      text: "Exit"
    }
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

  WorldMapEditor {
    visible: header.currentTab == "worldmap"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
    gameController: game
    worldMap: game.worldmap
  }

  DiplomacyEditor {
    visible: header.currentTab == "diplomacy"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
    dataEngine: game.dataEngine
  }

  DialogEditor {
    visible: header.currentTab == "dialogs"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
  }
}
