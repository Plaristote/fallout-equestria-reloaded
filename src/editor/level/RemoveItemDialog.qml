import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../assets/ui" as UiStyle
import "../../ui"

UiStyle.CustomDialog {
  property QtObject inventory
  property QtObject selectedObject

  title: i18n.t("game-editor.inventories.remove-items")
  modal: true
  anchors.centerIn: parent
  standardButtons: Dialog.Ok | Dialog.Cancel
  GridLayout {
    width: parent.width
    columns: 2
    CustomLabel { text: i18n.t("How much ?") }
    CustomTextField {
      id: removeItemQuantityInput
      text: "1"
      Layout.fillWidth: true
    }
  }
  onAccepted: {
    const itemType = selectedObject.objectName;
    inventory.removeItemOfType(itemType, parseInt(removeItemQuantityInput.text));
  }
}
