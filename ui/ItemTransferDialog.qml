import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

UiStyle.CustomDialog {
  property QtObject inventoryItem
  property int maxQuantity: inventoryItem ? inventoryItem.quantity : 1

  title: i18n.t("How much ?")
  modal: true
  standardButtons: Dialog.Ok | Dialog.Cancel

  signal pickedValue(int amount)

  Row {
    SpinBox {
      id: quantityInputField
      editable: true
      height: 40
      from: 1
      to: maxQuantity
      contentItem: CustomTextField {
        text: quantityInputField.value
        onTextChanged: quantityInputField.value = parseInt(text)
      }
    }
    CustomLabel {
      text: "/" + maxQuantity
    }
  }

  onAccepted: pickedValue(quantityInputField.value)
}
