import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
  property QtObject inventory
  property QtObject selectedObject

  title: "Remove items"
  modal: true
  anchors.centerIn: parent
  standardButtons: Dialog.Ok | Dialog.Cancel
  GridLayout {
    columns: 2
    Label { text: "How much" }
    TextField { id: removeItemQuantityInput; text: "1" }
  }
  onAccepted: {
    const itemType = selectedObject.objectName;
    inventory.removeItemOfType(itemType, parseInt(removeItemQuantityInput.text));
  }
}
