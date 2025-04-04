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
        dragZone: root
        onItemSelected: {
          playerStash.selectedObject = npcStash.selectedObject = npcInventory.selectedObject = null;
          selectedObject = selectedItem
        }
        onItemDropped: {
          if (controller.playerStash.contains(inventoryItem) >= 0)
            playerBarterControls.triggerTransferToLeft();
        }
      }

      BarterTransferControls {
        id: playerBarterControls
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: 50
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
        dragZone: root
        onItemSelected: {
          playerInventory.selectedObject = npcStash.selectedObject = npcInventory.selectedObject = null;
          selectedObject = selectedItem
        }
        onItemDropped: {
          if (controller.player.inventory.contains(inventoryItem) >= 0)
            playerBarterControls.triggerTransferToRight();
        }
      }

      BarterDealPreview {
        Layout.fillHeight: true
        Layout.preferredWidth: 300
        controller: root.controller
        playerItem: playerInventory.selectedObject || playerStash.selectedObject
        npcItem:    npcStash.selectedObject || npcInventory.selectedObject
      }

      InventoryItemsView {
        id: npcStash
        Layout.fillHeight: true
        Layout.fillWidth: true
        inventory: controller.npcStash
        dragZone: root
        onItemSelected: {
          playerInventory.selectedObject = playerStash.selectedObject = npcInventory.selectedObject = null;
          selectedObject = selectedItem
        }
        onItemDropped: {
          if (controller.npcInventory.contains(inventoryItem) >= 0)
            npcBarterControls.triggerTransferToLeft();
        }
      }

      BarterTransferControls {
        id: npcBarterControls
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: 50
        leftInventory:  npcStash
        rightInventory: npcInventory
        onTransferToLeft: controller.moveToNpcStash(item, amount)
        onTransferToRight: controller.moveToNpcInventory(item, amount)
      }

      ColumnLayout {
        SelectBox {
          visible: controller.npcInventoryTitles.length > 1
          model: controller.npcInventoryTitles
          onCurrentIndexChanged: controller.setCurrentNpcInventory(currentIndex)
          Layout.fillWidth: true
        }

        InventoryItemsView {
          id: npcInventory
          Layout.fillHeight: true
          Layout.fillWidth: true
          inventory: controller.npcInventory
          dragZone: root
          onItemSelected: {
            playerInventory.selectedObject = playerStash.selectedObject = npcStash.selectedObject = null;
            selectedObject = selectedItem
          }
          onItemDropped: {
            if (controller.npcStash.contains(inventoryItem) >= 0)
              npcBarterControls.triggerTransferToRight();
          }
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
        onClicked: root.abortBarter()
      }
    }
  }
}
