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
  withOrientation: true
  readOnlyAnimation: true
  readOnlyPositionType: true

  signal openInventoryClicked()
  signal requestCharacterView(string characterSheet)
  signal requestDialogView(string dialogName)

  fields: [
    TerminalLabel { text: "Character Sheet" },
    CharacterSheetInputField {
      Layout.fillWidth: true
      model: characterEditor.model
    },

    TerminalLabel { text: "Dialog" },
    RowLayout {
      Layout.fillWidth: true
      TerminalButton {
        Layout.fillWidth: true
        text: {
          if (characterEditor.model.scriptName.length > 0 && characterEditor.model.getDialogName() != undefined)
            return characterEditor.model.getDialogName();
          return "N/A";
        }
        enabled: false
      }
      TerminalToolButton {
        iconName: "open"
        onClicked: characterEditor.requestDialogView(characterEditor.model.getDialogName())
      }
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
