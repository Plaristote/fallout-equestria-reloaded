import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.0
import "qrc:/assets/ui" as UiStyle
import "../ui"

Pane {
  id: levelHud
  property QtObject gameController;
  property QtObject levelController;

  height: 135
  width: Math.min(parent.width, 1025)
  background: UiStyle.PlayPanel {}

  signal openMenu()
  signal openInventory()

  Pane {
    id: terminalPane
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: 240
    background: UiStyle.TerminalPane {}

    Flickable {
      id: terminalFlickable
      anchors.fill: parent
      contentHeight: terminalContent.height
      clip: true

      ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

      Column {
        id: terminalContent
        width: parent.width - 10
        onHeightChanged: terminalFlickable.contentY = Math.max(0, terminalContent.height - terminalFlickable.height);

        Repeater {
          model: gameController.consoleMessages
          delegate: Text {
            wrapMode: Text.WordWrap
            width: terminalContent.width
            text: "> " + gameController.consoleMessages[index]
            color: "green"
            font.family: application.consoleFontName
            font.pointSize: 6
          }
        }
      }
    }
  }

  Pane {
    id: menuPane
    anchors.left: terminalPane.right
    anchors.top: parent.top;
    anchors.bottom: parent.bottom
    anchors.topMargin: 20
    anchors.bottomMargin: 1
    anchors.leftMargin: 5
    width: 80
    background: UiStyle.Pane {}

    Column {
      anchors.fill: parent
      //anchors.topMargin: 10
      spacing: 5

      UiStyle.TinyButton {
        text: "INV"
        height: 20
        width: parent.width
        onClicked: openInventory()
      }

      UiStyle.TinyButton {
        text: "CHA"
        height: 20
        width: parent.width
        onClicked: {
          application.pushView("game/CharacterView.qml", {gameController: root.gameController})
        }
      }

      UiStyle.TinyButton {
        text: "Q"
        height: 20
        width: parent.width
        onClicked: openMenu()
      }
    }
  }

  Connections {
    target: levelController.player.inventory
    function onEquippedItemsChanged() {
      refreshUsableSlots(itemSlotsRepeater);
    }
  }

  function refreshUsableSlots(target) {
    const inventory   = levelController.player.inventory;
    const slotNames   = inventory.slotNames;
    const usableSlots = [];

    for (var i = 0 ; i < slotNames.length ; ++i) {
      if (slotNames[i].startsWith("use-"))
        usableSlots.push(slotNames[i]);
    }
    target.model = [];
    target.model = usableSlots;
  }

  Item {
    property QtObject inventory: levelController.player.inventory
    property int focusedItem: 0
    id: itemSlotsPane
    anchors.left: menuPane.right
    anchors.leftMargin: 5
    anchors.top: parent.top
    anchors.topMargin: 20
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 1

    Component.onCompleted: refreshUsableSlots(itemSlotsRepeater);

    Row {
      anchors.top: parent.top
      anchors.bottom: parent.bottom

      Repeater {
        id: itemSlotsRepeater
        model: itemSlotsPane.inventory.slotNames
        delegate: Button {
          property string slotName: itemSlotsRepeater.model[index]
          property QtObject slotItem: itemSlotsPane.inventory.getEquippedItem(slotName)

          anchors { top: parent.top; bottom: parent.bottom }
          width: 200
          hoverEnabled: true
          background: BorderImage {
            source: "qrc:/assets/ui/itemSlot.png"
            border { left: 4; bottom: 4; right: 4; top: 4 }
          }
          ColorOverlay {
            anchors.fill: parent
            source: background
            visible: parent.hovered
            color: Qt.rgba(155, 155, 155, 0.3)
          }
          Text {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 5
            font.family: application.titleFontName
            font.pixelSize: 14
            color: "yellow"
            text: slotItem ? slotItem.objectName : ""
          }
          Text {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 5
            font.family: application.titleFontName
            font.pixelSize: 14
            color: "yellow"
            text: slotItem ? slotItem.getActionPointCost() + " AP" : ""
          }
          ItemIcon {
            anchors.centerIn: parent
            model: slotItem
          }
          onClicked: levelController.setActiveItem(slotItem)
        } // END Button
      } // END Repeater
    } // END Row
  }
}
