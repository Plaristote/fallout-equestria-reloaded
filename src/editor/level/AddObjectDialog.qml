import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../assets/ui" as UiStyle
import "../../ui"
import "." as LevelEditor
import Game 1.0

UiStyle.CustomDialog {
  id: dialogAddObject
  property QtObject objectGroup
  property string error

  signal objectAdded(QtObject newObject)

  function openWithGroup(group) {
    objectGroup = group;
    open();
    objectTypeInput.forceActiveFocus();
  }

  function resetInputs() {
    objectNameInput.text = error = "";
    itemTypeInput.model = itemLibrary.getObjectList();
    sheetInput.model = scriptController.getCharacterSheets()
  }

  function validate() {
    error = objectGroup.validateObjectName(objectNameInput.text);
    return error.length === 0;
  }

  title: i18n.t("game-editor.levels.add-object")
  modal: true
  anchors.centerIn: parent
  standardButtons: Dialog.Ok | Dialog.Cancel
  background: UiStyle.Pane {}
  onOpened: resetInputs();
  GridLayout {
    columns: 2
    width: parent.width
    CustomLabel { text: i18n.t("game-editor.levels.object-type") }
    SelectBox {
      id: objectTypeInput
      valueRole: "value"
      textRole: "text"
      model: [
        {value: "character", text: i18n.t("game-editor.levels.objects.types.character")},
        {value: "storage", text: i18n.t("game-editor.levels.objects.types.storage")},
        {value: "door", text: i18n.t("game-editor.levels.objects.types.door")},
        {value: "elevator", text: i18n.t("game-editor.levels.objects.types.elevator")},
        {value: "item", text: i18n.t("game-editor.levels.objects.types.item")},
        {value: "other", text: i18n.t("game-editor.levels.objects.types.other")}
      ]
      Layout.fillWidth: true
      Layout.preferredHeight: 40
    }

    CustomLabel { text: i18n.t("game-editor.levels.object-name") }
    CustomTextField {
      id: objectNameInput
      Layout.fillWidth: true
      Layout.preferredHeight: 40
    }

    // Character inputs
    CustomLabel { text: i18n.t("game-editor.levels.characters.sheet"); visible: objectTypeInput.currentValue === "character" }
    TerminalButton {
      id: sheetInput
      Layout.fillWidth: true
      Layout.preferredHeight: 40
      visible: objectTypeInput.currentValue === "character"
      onClicked: sheetPicker.startPicking(text)
      LevelEditor.CharacterSheetPicker {
        id: sheetPicker
        parent: Overlay.overlay
        anchors.centerIn: parent
        onAccepted: sheetInput.text = pickedOption
      }
    }

    // InventoryItem inputs
    CustomLabel { text: i18n.t("game-editor.item"); visible: objectTypeInput.currentValue === "item" }
    SelectBox {
      id: itemTypeInput
      visible: objectTypeInput.currentValue === "item"
      Layout.fillWidth: true
      Layout.preferredHeight: 40
    }

    CustomLabel { text: i18n.t("game-editor.error"); visible: error.length > 0 }
    CustomLabel { text: error; visible: error.length > 0 }
  }

  onAccepted: {
    const factory = objectGroup.factory();
    var newObject;

    if (objectTypeInput.currentValue === "character")
      newObject = factory.generateCharacter(objectNameInput.text, sheetInput.text.replace(".json", ""));
    else if (objectTypeInput.currentValue == "storage")
      newObject = factory.generateStorageObject(objectNameInput.text);
    else if (objectTypeInput.currentValue == "door")
      newObject = factory.generateDoorway(objectNameInput.text);
    else if (objectTypeInput.currentValue == "elevator")
      newObject = factory.generateElevator(objectNameInput.text);
    else if (objectTypeInput.currentValue == "item")
      newObject = factory.generateInventoryItem(objectNameInput.text, itemTypeInput.currentText, 1);
    else
      newObject = factory.generateDynamicObject(objectNameInput.text);
    objectAdded(newObject);
    resetInputs();
  }
}
