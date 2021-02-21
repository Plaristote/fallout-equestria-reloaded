import QtQuick 2.15
import QtQuick.Controls 2.15
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

  HudConsole {
    id: terminalPane
    gameController: levelHud.gameController
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: 240
  }

  HudMenu {
    id: menuPane
    anchors.left: terminalPane.right
    anchors.top: parent.top;
    anchors.bottom: parent.bottom
    anchors.topMargin: 20
    anchors.bottomMargin: 1
    anchors.leftMargin: 5
    width: 80
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
    anchors.topMargin: 20
  }

  HudItemSlots {
    id: itemSlotsPane
    inventory: levelController.player.inventory
    anchors.left: menuPane.right
    anchors.leftMargin: 5
    anchors.top: actionPointPane.bottom
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 1
    onItemActivated: levelController.setActiveItem(activatedItem)
  }

  Row {
    anchors.bottom: actionPointPane.top
    anchors.bottomMargin: 10
    anchors.horizontalCenter: actionPointPane.horizontalCenter
    clip: true
    spacing: 10
    Repeater {
      model: levelController.combattants
      delegate: Rectangle {
        property QtObject character: levelController.combattants[index]

        height: 20
        width: 20
        border.width: 2
        border.color: levelController.turn === index ? "yellow" : "black"
        color: character === levelController.player ? "lightgreen" : "blue"
      }
    }
  }

  Column {
    anchors.left: actionPointPane.right
    anchors.leftMargin: 20
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
}
