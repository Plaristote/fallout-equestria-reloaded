import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"
import "../../game" as GameComponents
import Game 1.0

DynamicObjectEditor {
  id: itemEditor
  scriptCategory: "items"
  readOnlySprite: true
  readOnlyAnimation: true
  readOnlyPositionType: true
  readOnlyScript: true
  withInteractionPosition: false

  fields: [
    TerminalLabel { text: "Type" },
    TerminalComboBox {
      Layout.fillWidth: true
      model: itemLibrary.getObjectList()
      currentIndex: itemLibrary.getObjectList().indexOf(model.itemType)
      onCurrentIndexChanged: {
        model.itemType = itemLibrary.getObjectList()[currentIndex];
      }
    },

    TerminalLabel { text: "Quantity" },
    TerminalField {
      Layout.fillWidth: true
      text: model.quantity.toString()
      onTextChanged: {
        model.quantity = parseInt(text)
      }
    }
  ]
}
