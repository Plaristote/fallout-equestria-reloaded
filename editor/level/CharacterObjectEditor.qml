import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"
import "../../game" as GameComponents
import Game 1.0

DynamicObjectEditor {
  id: characterEditor
  scriptCategory: "pnjs"
  readOnlyAnimation: true
  readOnlyPositionType: true

  signal openInventoryClicked()
  signal requestCharacterView(string characterSheet)

  fields: [
    TerminalLabel { text: "Character Sheet" },
    RowLayout {
      Layout.fillWidth: true
      TerminalComboBox {
        Layout.fillWidth: true
        model: scriptController.getCharacterSheets()
        currentIndex: scriptController.getCharacterSheets().indexOf(characterEditor.model.characterSheet + ".json")
        onCurrentIndexChanged: {
          characterEditor.model.characterSheet = scriptController.getCharacterSheets()[currentIndex].replace(".json", "");
        }
      }
      TerminalToolButton {
        iconName: "open"
        onClicked: characterEditor.requestCharacterView(characterEditor.model.characterSheet)
      }
    },

    TerminalLabel { text: "Dialog" },
    TerminalButton {
      Layout.fillWidth: true
      text: {
        if (characterEditor.model.scriptName.length > 0 && characterEditor.model.getDialogName() != undefined)
          return characterEditor.model.getDialogName();
        return "N/A";
      }
      enabled: text !== "N/A"
      onClicked: console.log("TODO: jump to dialog editor", characterEditor.model.getDialogName(), characterEditor.model.scriptName.length);
    },

    TerminalLabel { text: "Faction" },
    TerminalField {
      Layout.fillWidth: true
      text: model.statistics.faction
      onTextChanged: {
        model.statistics.faction = text
      }
    },

    TerminalLabel { text: "Unique" },
    TerminalCheckBox {
      checked: model.isUnique
      onCheckedChanged: {
        model.isUnique = checked
      }
    },

    TerminalToolButton {
      Layout.columnSpan: 2
      Layout.fillWidth: true
      Layout.preferredHeight: 40
      text: "Inventory"
      iconName: "item"
      onClicked: openInventoryClicked();
      padding: 20
      height: 40
    }
  ]
}
