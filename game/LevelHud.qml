import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "./hud"

Pane {
  id: levelHud
  property QtObject gameController;
  property QtObject levelController;

  height: 135
  width: Math.min(parent.width, 1025)
  background: UiStyle.PlayPanel {}

  signal openMenu()
  signal openInventory()
  signal openSkilldex()
  signal openSpellbook()

  HudConsole {
    id: terminalPane
    gameController: levelHud.gameController
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.margins: -4
    width: 275
  }

  HudMenu {
    id: menuPane
    anchors.left: terminalPane.right
    anchors.top: parent.top;
    anchors.bottom: parent.bottom
    anchors.topMargin: 25
    anchors.bottomMargin: 1
    anchors.leftMargin: 5
    width: 50
    onOpenInventory: levelHud.openInventory()
    onOpenMenu: levelHud.openMenu()
  }

  HudActionPoints {
    id: actionPointPane
    maxActionPoints: levelController.player.statistics.actionPoints
    actionPoints: levelController.player.actionPoints
    anchors.left: menuPane.right
    width: 400
    anchors.top: parent.top
    anchors.topMargin: 10
  }

  HudItemSlots {
    id: itemSlotsPane
    inventory: levelController.player.inventory
    anchors.left: menuPane.right
    anchors.leftMargin: 5
    anchors.top: actionPointPane.bottom
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 1
    activeItem: levelController.activeItem
    onItemActivated: levelController.setActiveItem(activatedSlot)
  }

  CombatPreview {
    levelController: root.levelController
    anchors {
      bottom: actionPointPane.top
      bottomMargin: 10
      horizontalCenter: actionPointPane.horizontalCenter
    }
  }

  HudCombatControls {
    id: combatControls
    anchors {
      bottom: parent.bottom
      right: countersDisplay.left
      top: parent.top
      rightMargin: 10
      topMargin: 20
    }
    width: 100
  }

  Column {
    id: countersDisplay
    anchors.right: rightMenu.left
    anchors.rightMargin: 10
    anchors.bottom: parent.bottom

    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      text: "HP"
      color: "white"
      font.family: application.titleFontName
    }

    HudCounter {
      source: levelController.player.statistics.hitPoints
    }

    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      text: "AP"
      color: "white"
      font.family: application.titleFontName
    }

    HudCounter {
      source: levelController.player.statistics.armorClass
    }
  }

  ColumnLayout {
    id: rightMenu
    anchors {
      right: parent.right
      bottom: parent.bottom
      margins: 5
    }

    spacing: 2

    UiStyle.TinyButton {
      text: "skilldex"
      Layout.fillWidth: true
      onClicked: openSkilldex()
    }

    UiStyle.TinyButton {
      text: "spellbook"
      Layout.fillWidth: true
      onClicked: openSpellbook()
    }

    UiStyle.TinyButton {
      text: "Pipboy"
      Layout.fillWidth: true
      onClicked: application.pushView("game/PipBoy.qml", {gameController: gameController, levelController: levelController})
    }
  }
}
