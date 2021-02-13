import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/ui"
import "qrc:/assets/ui" as UiStyle

Pane {
  id: root
  property QtObject controller
  property QtObject selectedInventory
  property QtObject selectedObject

  background: UiStyle.Pane {}

  Connections {
    target: controller
    function onFinished() {
      application.popView();
      gameManager.currentGame.level.paused = false;
    }
  }

  Action {
    id: takeAllAction
    onTriggered: {
      console.log("TODO: implement take all");
    }
  }

  Action {
    id: closeAction
    onTriggered: {
      controller.finished();
    }
  }

  Action {
    id: takeAction
    enabled: selectedObject != null && selectedInventory == controller.inventory
    onTriggered: {
      if (selectedObject.quantity > 1)
        quantityInputDialog.open();
      else
      {
        controller.take(selectedObject, 1);
        selectedObject = null;
      }
    }
  }

  Action {
    id: dropAction
    enabled: selectedObject != null && selectedInventory == controller.character.inventory
    onTriggered: {
      if (selectedObject.quantity > 1)
        quantityInputDialog.open();
      else
      {
        controller.drop(selectedObject, 1);
        selectedObject = null;
      }
    }
  }

  Dialog {
    id: quantityInputDialog
    title: "How much ?"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel

    Row {
      SpinBox {
        id: quantityInputField
        editable: true
      }
      Text {
        text: "/" + selectedObject.quantity
      }
    }

    onAccepted: {
      if (selectedInventory == controller.character.inventory)
        controller.drop(selectedObject, quantityInputField.value);
      else
        controller.take(selectedObject, quantityInputField.value);
      if (selectedObject.quantity == quantityInputField.value)
        selectedObject = null;
    }
  }

  RowLayout {
    anchors { top: parent.top; left: parent.left; bottom: controls.top; right: parent.right }
    anchors.margins: 15

    InventoryItemsView {
      id: characterInventoryView
      inventory: controller.character.inventory
      selectedObject: root.selectedObject
      onItemSelected: {
        root.selectedInventory = controller.character.inventory;
        root.selectedObject    = selectedItem;
      }
      Layout.preferredWidth: parent.width / 3
      Layout.fillHeight: true
    }

    Pane {
      background: UiStyle.Pane {}
      Layout.preferredWidth: parent.width / 3
      Layout.maximumWidth: 300
      Layout.fillHeight: true

      Column {
        anchors.fill: parent
        anchors.margins: 10

        Pane {
          background: UiStyle.TerminalPane {}
          height: 50;
          anchors.left: parent.left
          anchors.right: parent.right
          anchors.margins: 10
        }

        MenuButton {
          text: "Take"
          action: takeAction
          anchors.horizontalCenter: parent.horizontalCenter
        }
        MenuButton {
          text: "Leave"
          action: dropAction
          anchors.horizontalCenter: parent.horizontalCenter
        }
      }
    }

    InventoryItemsView {
      id: targetInventoryView
      inventory: controller.inventory
      selectedObject: root.selectedObject
      onItemSelected: {
        root.selectedInventory = controller.inventory
        root.selectedObject    = selectedItem
      }

      Layout.preferredWidth: parent.width / 3
      Layout.fillHeight: true
    }
  }

  Row {
    id: controls
    anchors { bottom: parent.bottom; right: parent.right }

    MenuButton {
      text: "Take all"
      action: takeAllAction
    }

    MenuButton {
      text: "Close"
      action: closeAction
    }
  }
}
