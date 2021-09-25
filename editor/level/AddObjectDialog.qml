import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../assets/ui" as UiStyle
import "../../ui"
import Game 1.0

UiStyle.CustomDialog {
  id: dialogAddObject
  property QtObject objectGroup

  signal objectAdded(QtObject newObject)

  function openWithGroup(group) {
    objectGroup = group;
    open();
    objectTypeInput.forceActiveFocus();
  }

  function resetInputs() {
    objectNameInput.text = "";
    itemTypeInput.model = itemLibrary.getObjectList();
    sheetInput.model = scriptController.getCharacterSheets()
  }

  title: "New object"
  modal: true
  anchors.centerIn: parent
  standardButtons: Dialog.Ok | Dialog.Cancel
  background: UiStyle.Pane {}
  onOpened: resetInputs();
  GridLayout {
    columns: 2
    width: parent.width
    CustomLabel { text: "Type" }
    SelectBox {
      id: objectTypeInput
      model: ["character", "storage", "door", "item", "other"]
      Layout.fillWidth: true
      Layout.preferredHeight: 40
    }

    CustomLabel { text: "Name" }
    CustomTextField {
      id: objectNameInput
      Layout.fillWidth: true
      Layout.preferredHeight: 40
    }

    // Character inputs
    CustomLabel { text: "Character sheet"; visible: objectTypeInput.currentText === "character" }
    SelectBox {
      id: sheetInput;
      visible: objectTypeInput.currentText === "character"
      Layout.fillWidth: true
      Layout.preferredWidth: 40
    }

    // InventoryItem inputs
    CustomLabel { text: "Item"; visible: objectTypeInput.currentText === "item" }
    SelectBox {
      id: itemTypeInput
      visible: objectTypeInput.currentText === "item"
      Layout.fillWidth: true
      Layout.preferredHeight: 40
    }
  }

  onAccepted: {
    const factory = objectGroup.factory();
    var newObject;

    if (objectTypeInput.currentText === "character")
      newObject = factory.generateCharacter(objectNameInput.text, sheetInput.currentText.replace(".json", ""));
    else if (objectTypeInput.currentText == "storage")
      newObject = factory.generateStorageObject(objectNameInput.text);
    else if (objectTypeInput.currentText == "door")
      newObject = factory.generateDoorway(objectNameInput.text);
    else if (objectTypeInput.currentText == "item")
      newObject = factory.generateInventoryItem(objectNameInput.text, itemTypeInput.currentText);
    else
      newObject = factory.generateDynamicObject(objectNameInput.text);
    objectAdded(newObject);
    resetInputs();
  }
}
