import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../ui"

CharacterInventory {
  id: characterInventory
  title: `${i18n.t("game-editor.inventory")} - ${character.objectName}`

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
    {label: i18n.t("game-editor.inventories.add-item"), callback: characterInventory.addObject},
    {label: i18n.t("game-editor.inventories.remove-item"), callback: characterInventory.removeObject}
  ]

  onClosed: { character = null; visible = false }

  AddItemDialog {
    id: addItemDialog
    inventory: characterInventory.character.inventory
  }

  RemoveItemDialog {
    id: removeItemDialog
    inventory: characterInventory.character.inventory
    selectedObject: characterInventory.selectedObject
  }
}
