import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../../foengine/ui/style" as UiStyle
import "../../../foengine/ui"

GridLayout {
  property alias objectType: objectTypeInput.currentText
  property alias characterSheet: sheetInput.currentText
  property alias itemType: itemTypeInput.currentText

  function loadFromTemplate(name) {
    sheetInput.currentIndex = sheetInput.indexOfValue(gameObjectTemplates.getCharacterSheet(name));
    itemTypeInput.currentIndex = sheetInput.indexOfValue(gameObjectTemplates.getObjectType(name));
    objectNameInput.forceActiveFocus()
  }

  columns: 2
  width: parent.width
  TerminalLabel { text: "Type" }
  TerminalComboBox {
    id: objectTypeInput
    model: ["character", "storage", "door", "item", "other"]
    Layout.fillWidth: true
    Layout.preferredHeight: 40
  }

  TerminalLabel { text: "Name" }
  TerminalField {
    id: objectNameInput
    placeholderText: "Insert new object name here"
    Layout.fillWidth: true
    Layout.preferredHeight: 40
  }

  // Character inputs
  TerminalLabel { text: "Character sheet"; visible: objectType === "character" }
  TerminalComboBox {
    id: sheetInput;
    visible: objectTypeInput.currentText === "character"
    Layout.fillWidth: true
    Layout.preferredWidth: 40
  }

  // InventoryItem inputs
  TerminalLabel { text: "Item"; visible: objectType === "item" }
  TerminalComboBox {
    id: itemTypeInput
    visible: objectTypeInput.currentText === "item"
    Layout.fillWidth: true
    Layout.preferredHeight: 40
  }
}
