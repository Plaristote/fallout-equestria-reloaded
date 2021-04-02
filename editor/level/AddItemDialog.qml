import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../assets/ui" as UiStyle
import "../../ui"

UiStyle.CustomDialog {
  property QtObject inventory

  title: "Add item"
  modal: true
  anchors.centerIn: parent
  standardButtons: Dialog.Ok | Dialog.Cancel
  GridLayout {
    width: parent.width
    columns: 2
    CustomLabel { text: "Object" }
    SelectBox {
      id: newItemSelect
      model: itemLibrary.getObjectList()
      Layout.fillWidth: true
      Layout.preferredHeight: 40
    }

    CustomLabel { text: "Quantity" }
    CustomTextField {
      id: newItemQuantityInput
      text: "1"
      Layout.fillWidth: true
      Layout.preferredHeight: 40
    }
  }
  onAccepted: {
    inventory.addItemOfType(newItemSelect.currentText, parseInt(newItemQuantityInput.text));
  }
}
