import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

UiStyle.CustomDialog {
  property QtObject inventoryItem

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
      to: inventoryItem.quantity
      contentItem: CustomTextField {
        text: quantityInputField.value
        onTextChanged: quantityInputField.value = parseInt(text)
      }
    }
    CustomLabel {
      text: "/" + inventoryItem.quantity
    }
  }

  onAccepted: pickedValue(quantityInputField.value)
}
