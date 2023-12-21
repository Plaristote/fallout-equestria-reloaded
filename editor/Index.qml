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

  function goToSprite(group) {
    header.currentTab = "sprites";
    spriteEditor.animationGroup = group;
  }

  function goToNpc(characterSheet) {
    header.currentTab = "npcs";
    npcEditor.currentCharacter = characterSheet + ".json";
  }

  function goToDialog(dialogName) {
    header.currentTab = "dialogs";
    dialogEditor.currentDialog = dialogName + ".json";
  }

  TabRow {
    id: header
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    tabs: ["worldmap", "levels", "npcs", "dialogs", "diplomacy", "objects", "sprites"]
    labels: tabs.map(tabname => i18n.t(`game-editor.tabs.${tabname}`));
    currentTab: tabs[0]
  }

  Button {
    background: UiStyle.Label { style: parent.down ? "dark" : "base" }
    anchors.right: parent.right
    anchors.top: parent.top
    contentItem: Text {
      color: "white"
      text: i18n.t("game-editor.exit")
    }
    onClicked: application.popView()
  }

  SpriteEditor {
    id: spriteEditor
    visible: header.currentTab == "sprites"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
  }

  ObjectEditor {
    visible: header.currentTab == "objects"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
  }

  NpcEditor {
    id: npcEditor
    visible: header.currentTab == "npcs"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
  }

  LevelEditor {
    visible: header.currentTab == "levels"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
    gameController: game
    onRequestCharacterView: goToNpc(characterSheet)
    onRequestSpriteView: goToSprite(group)
    onRequestDialogView: goToDialog(dialogName)
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
    id: dialogEditor
    visible: header.currentTab == "dialogs"
    anchors { top: header.bottom; left: parent.left; right: parent.right; bottom: parent.bottom }
  }
}
