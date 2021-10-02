import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15
import "../ui"
import "../ui/dialog"
import "qrc:/assets/ui" as UiStyle

Item {
  id: root
  property QtObject controller

  signal closed()

  function makeDeal() {
    if (controller.agreeToBarter())
      controller.concludeBarter();
  }

  function abortBarter() {
    controller.closeBarter();
    closed();
  }

  Pane {
    background: UiStyle.Pane {}
    anchors { left: parent.left; right: barterRightPane.left; top: parent.top; bottom: parent.bottom }

    RowLayout {
      anchors.fill: parent

      InventoryItemsView {
        id: playerInventory
        Layout.fillHeight: true
        Layout.fillWidth: true
        inventory: controller.player.inventory
        onItemSelected: {
          playerStash.selectedObject = npcStash.selectedObject = npcInventory.selectedObject = null;
          selectedObject = selectedItem
        }
      }

      BarterTransferControls {
        Layout.alignment: Qt.AlignHCenter
        leftInventory:  playerInventory
        rightInventory: playerStash
        onTransferToLeft: controller.moveToPlayerInventory(item, amount)
        onTransferToRight: controller.moveToPlayerStash(item, amount)
      }

      InventoryItemsView {
        id: playerStash
        Layout.fillHeight: true
        Layout.fillWidth: true
        inventory: controller.playerStash
        onItemSelected: {
          playerInventory.selectedObject = npcStash.selectedObject = npcInventory.selectedObject = null;
          selectedObject = selectedItem
        }
      }

      BarterDealPreview {
        Layout.alignment: Qt.AlignVCenter
        Layout.preferredWidth: 200
        controller: root.controller
      }

      InventoryItemsView {
        id: npcStash
        Layout.fillHeight: true
        Layout.fillWidth: true
        inventory: controller.npcStash
        onItemSelected: {
          playerInventory.selectedObject = playerStash.selectedObject = npcInventory.selectedObject = null;
          selectedObject = selectedItem
        }
      }

      BarterTransferControls {
        Layout.alignment: Qt.AlignHCenter
        leftInventory:  npcStash
        rightInventory: npcInventory
        onTransferToLeft: controller.moveToNpcStash(item, amount)
        onTransferToRight: controller.moveToNpcInventory(item, amount)
      }

      InventoryItemsView {
        id: npcInventory
        Layout.fillHeight: true
        Layout.fillWidth: true
        inventory: controller.npc.inventory
        onItemSelected: {
          playerInventory.selectedObject = playerStash.selectedObject = npcStash.selectedObject = null;
          selectedObject = selectedItem
        }
      }
    }
  }

  Image {
    id: barterRightPane
    source: "qrc:/assets/ui/dialog/right.png"
    anchors { top: parent.top; bottom: parent.bottom; right: parent.right; }
    width: 132
    fillMode: Image.Stretch

    Column {
      anchors.top: parent.top
      anchors.topMargin: 20
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: 25

      UiStyle.PushButton {
        text: i18n.t("Deal")
        onClicked: root.makeDeal()
      }

      UiStyle.PushButton {
        text: i18n.t("Close")
        onClicked: root.closed()
      }
    }
  }
}
