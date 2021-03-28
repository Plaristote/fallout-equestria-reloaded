import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
  property QtObject inventory

  title: "Add item"
  modal: true
  anchors.centerIn: parent
  standardButtons: Dialog.Ok | Dialog.Cancel
  GridLayout {
    columns: 2
    Label { text: "Object" }
    ComboBox { id: newItemSelect; model: itemLibrary.getObjectList() }
    Label { text: "Quantity" }
    TextField { id: newItemQuantityInput; text: "1" }
  }
  onAccepted: {
    inventory.addItemOfType(newItemSelect.currentText, parseInt(newItemQuantityInput.text));
  }
}
