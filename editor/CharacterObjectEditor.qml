import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"
import "../game" as GameComponents
import Game 1.0

DynamicObjectEditor {
  id: characterEditor
  scriptCategory: "pnjs"

  signal openInventoryClicked()

  fields: [
    TerminalLabel { text: "Dialog" },
    TerminalComboBox {
      model: scriptController.getScripts("dialogs")
      enabled: false
      currentIndex: scriptController.getScript("dialogs").indexOf(characterEditor.model.getDialog())
    },

    TerminalButton {
      Layout.columnSpan: 2
      Layout.alignment: Qt.AlignCenter
      text: "Inventory"
      onClicked: openInventoryClicked()
      padding: 20
      height: 40
    }
  ]
}
