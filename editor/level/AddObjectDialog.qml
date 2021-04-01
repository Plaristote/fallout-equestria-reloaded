import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../assets/ui" as UiStyle
import "../../ui"
import Game 1.0

UiStyle.CustomDialog {
  id: dialogAddObject
  property QtObject gameController

  signal objectAdded(QtObject newObject)

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
    Text { text: "Type"; color: "white"; font.family: application.titleFontName }
    ComboBox {
      id: objectTypeInput
      model: ["character", "storage", "door", "item", "other"]
      Layout.fillWidth: true
    }

    Text { text: "Name"; color: "white"; font.family: application.titleFontName }
    TextField {
      id: objectNameInput
      Layout.fillWidth: true
    }

    // Character inputs
    Text { text: "Character sheet"; color: "white"; font.family: application.titleFontName; visible: objectTypeInput.currentText === "character" }
    ComboBox {
      id: sheetInput;
      visible: objectTypeInput.currentText === "character"
      Layout.fillWidth: true
    }

    // InventoryItem inputs
    Text { text: "Item"; color: "white"; font.family: application.titleFontName; visible: objectTypeInput.currentText === "item" }
    ComboBox {
      id: itemTypeInput
      visible: objectTypeInput.currentText === "item"
      Layout.fillWidth: true
    }
  }

  onAccepted: {
    var newObject;

    if (objectTypeInput.currentText === "character")
      newObject = gameController.level.generateCharacter(objectNameInput.text, sheetInput.currentText.replace(".json", ""));
    else if (objectTypeInput.currentText == "storage")
      newObject = gameController.level.generateStorageObject(objectNameInput.text);
    else if (objectTypeInput.currentText == "door")
      newObject = gameController.level.generateDoorway(objectNameInput.text);
    else if (objectTypeInput.currentText == "item")
      newObject = gameController.level.generateInventoryItem(objectNameInput.text, itemTypeInput.currentText);
    else
      newObject = gameController.level.generateDynamicObject(objectNameInput.text);
    objectAdded(newObject);
    resetInputs();
  }
}
