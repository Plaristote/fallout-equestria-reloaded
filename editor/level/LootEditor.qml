import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../assets/ui" as UiStyle
import "../../ui"
import "../../game" as GameComponents

Pane {
  id: root
  property QtObject inventory
  property QtObject selectedObject

  background: UiStyle.Pane {}

  function open() {
    root.visible = true;
  }

  function addObject() {
    addItemDialog.open();
  }

  function removeObject() {
    if (selectedObject !== null) {
      if (selectedObject.quantity === 1)
        root.inventory.removeItem(selectedObject);
      else
        removeItemDialog.open();
    }
  }

  RowLayout {
    anchors.fill: parent
    anchors.margins: 10

    GameComponents.InventoryItemsView {
      id: itemsView
      inventory: root.inventory
      Layout.fillHeight: true
      Layout.fillWidth: true
      selectedObject: root.selectedObject
      onItemSelected: root.selectedObject = selectedItem
    }

    Column {
      spacing: 5

      Layout.preferredWidth: 300
      Loader {
        anchors { left: parent.left; right: parent.right }
        sourceComponent: root.selectedObject ? itemPreviewComponent : null
      }

      MenuButton {
        text: "Add item"
        onClicked: root.addObject()
      }

      MenuButton {
        text: "Remove item"
        enabled: root.selectedObject !== null
        onClicked: root.removeObject()
      }

      MenuButton {
        text: "Close"
        onClicked: {
          root.inventory = null;
          root.visible = false;
        }
      }
    }
  }

  AddItemDialog {
    id: addItemDialog
    inventory: root.inventory
  }

  RemoveItemDialog {
    id: removeItemDialog
    inventory: root.inventory
    selectedObject: root.selectedObject
  }

  Component {
    id: itemPreviewComponent
    InventoryItemPreview {
      model: root.selectedObject
    }
  }
}
