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
  width: Math.min(parent.width, 1070)
  background: UiStyle.PlayPanel {
    leftWidth: terminalPane.width + 45
  }

  signal openMenu()
  signal openInventory()
  signal openSkilldex()
  signal openSpellbook()
  signal openCharacterSheet()
  signal openPipboy()

  RowLayout {
    anchors { bottom: parent.top; bottomMargin: 10 }

    UiStyle.WarningRectangle {
      visible: levelController.player.statistics.hasLeveledUp
      label: i18n.t("cmap.level-up")
    }

    UiStyle.WarningRectangle {
      visible: levelController.player.unconscious
      label: i18n.t("cmap.buffs.ko")
    }

    UiStyle.WarningRectangle {
      visible: levelController.player.sneaking
      label: i18n.t("cmap.sneak")
    }
  }

  HudConsole {
    id: terminalPane
    gameController: levelHud.gameController
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.margins: -4
    width: 300
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
    onOpenCharacterSheet: levelHud.openCharacterSheet()
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
      text: i18n.t("hud.HP")
      color: "black"
      style: Text.Outline
      styleColor: "white"
      font.family: application.titleFontName
      font.pixelSize: 16
    }

    HudCounter {
      source: levelController.player.statistics.hitPoints
    }

    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      text: i18n.t("hud.AC")
      color: "black"
      style: Text.Outline
      styleColor: "white"
      font.family: application.titleFontName
      font.pixelSize: 16
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
      text: i18n.t("hud.skilldex")
      Layout.fillWidth: true
      onClicked: openSkilldex()
    }

    UiStyle.TinyButton {
      text: i18n.t("hud.spellbook")
      Layout.fillWidth: true
      onClicked: openSpellbook()
    }

    UiStyle.TinyButton {
      text: i18n.t("hud.pipboy")
      Layout.fillWidth: true
      onClicked: openPipboy()
    }
  }
}
