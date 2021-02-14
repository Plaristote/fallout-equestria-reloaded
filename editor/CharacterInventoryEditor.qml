import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../ui"

CharacterInventory {
  id: characterInventory
  title: "Inventory - " + character.objectName

  function open(value) {
    characterInventory.visible = true;
  }

  function addObject() {
    addItemDialog.open();
  }

  function removeObject() {
    if (selectedObject !== null) {
      if (selectedObject.quantity === 1)
        characterInventory.character.inventory.removeItem(selectedObject);
      else
        removeItemDialog.open();
    }
  }

  additionalControls: [
    {label: "Add", callback: characterInventory.addObject},
    {label: "Remove", callback: characterInventory.removeObject}
  ]

  onClosed: { character = null; visible = false }

  Dialog {
    id: addItemDialog
    title: "Add item"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    GridLayout {
      columns: 2
      Label { text: "Object" }
      ComboBox { id: newItemSelect; model: itemLibrary.getObjectList() }
      Label { text: "Quantity" }
      TextField { id: newItemQuantityInput; text: "1" }
    }
    onAccepted: {
      const inventory = characterInventory.character.inventory;
      inventory.addItemOfType(newItemSelect.currentText, parseInt(newItemQuantityInput.text));
    }
  }

  Dialog {
    id: removeItemDialog
    title: "Remove items"
    modal: true
    anchors.centerIn: parent
    standardButtons: Dialog.Ok | Dialog.Cancel
    GridLayout {
      columns: 2
      Label { text: "How much" }
      TextField { id: removeItemQuantityInput; text: "1" }
    }
    onAccepted: {
      const inventory = characterInventory.character.inventory;
      const itemType = characterInventory.selectedObject.objectName;
      inventory.removeItemOfType(itemType, parseInt(removeItemQuantityInput.text));
    }
  }
}
