import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15
import "../../ui"
import "qrc:/assets/ui" as UiStyle

Pane {
  property var leftInventory
  property var rightInventory

  signal transferToLeft(QtObject item, int amount)
  signal transferToRight(QtObject item, int amount)

  background: UiStyle.TerminalPane{}

  ItemTransferDialog {
    id: transferToRightDialog
    parent: Overlay.overlay
    anchors.centerIn: parent
    inventoryItem: leftInventory.selectedObject
    onPickedValue: {
      transferToRight(inventoryItem, amount);
      leftInventory.selectedObject = null;
    }
  }

  ItemTransferDialog {
    id: transferToLeftDialog
    parent: Overlay.overlay
    anchors.centerIn: parent
    inventoryItem: rightInventory.selectedObject
    onPickedValue: {
      transferToLeft(inventoryItem, amount);
      rightInventory.selectedObject = null;
    }
  }

  ColumnLayout {
    TerminalButton {
      Layout.alignment: Qt.AlignHCenter
      text: "<"
      onClicked: {
        if (rightInventory.selectedObject.quantity > 1)
          transferToLeftDialog.open();
        else
        {
          transferToLeft(rightInventory.selectedObject, -1);
          rightInventory.selectedObject = null;
        }
      }
    }
    TerminalButton {
      Layout.alignment: Qt.AlignHCenter
      text: ">"
      onClicked: {
        if (playerInventory.selectedObject.quantity > 1)
          transferToRightDialog.open();
        else
        {
          transferToRight(playerInventory.selectedObject, -1)
          leftInventory.selectedObject = null;
        }
      }
    }
  }
}
