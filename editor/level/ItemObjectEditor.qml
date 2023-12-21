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

  fields: [
    TerminalLabel { text: i18n.t("game-editor.items.type") },
    TerminalComboBox {
      Layout.fillWidth: true
      model: itemLibrary.getObjectList()
      currentIndex: itemLibrary.getObjectList().indexOf(itemEditor.model.itemType)
      onCurrentIndexChanged: {
        itemEditor.model.itemType = itemLibrary.getObjectList()[currentIndex];
      }
    },

    TerminalLabel { text: i18n.t("game-editor.items.quantity") },
    TerminalField {
      Layout.fillWidth: true
      text: model.quantity.toString()
      onTextChanged: {
        model.quantity = parseInt(text)
      }
    }
  ]
}
