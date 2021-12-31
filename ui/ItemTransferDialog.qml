import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

UiStyle.CustomDialog {
  property QtObject inventoryItem
  property int maxQuantity: inventoryItem ? inventoryItem.quantity : 1
  property int defaultValue: 1

  title: i18n.t("How much ?")
  modal: true
  standardButtons: Dialog.Ok | Dialog.Cancel

  signal pickedValue(int amount)

  RowLayout {
    anchors.centerIn: parent

    UiStyle.TinyButton {
      text: "-"
      enabled: quantityInputField.value > 1
      onClicked: quantityInputField.text = (quantityInputField.value - 1).toString()

      RepeatTimer {
        running: parent.down
        onRepeat: quantityInputField.text = (quantityInputField.value - 1).toString()
      }
    }

    UiStyle.TerminalPane {
      Layout.preferredWidth:  200
      Layout.preferredHeight: 100

      RowLayout {
        id: inputLayout
        anchors.centerIn: parent
        TerminalField {
          property int value: defaultValue
          id: quantityInputField
          validator: IntValidator { bottom: 1; top: maxQuantity }
          text: "1"
          font.pixelSize: 17
          onTextChanged: {
            const number = parseInt(text);

            if (number > maxQuantity) { text = maxQuantity.toString(); }
            else if (number < 1)      { text = "1"; }
            value = parseInt(text);
          }
        }

        TerminalLabel {
          text: "/" + maxQuantity
          font.pixelSize: 17
        }
      }
    }

    UiStyle.TinyButton {
      text: "+"
      enabled: quantityInputField.value < maxQuantity
      onClicked: quantityInputField.text = (quantityInputField.value + 1).toString()
      RepeatTimer {
        running: parent.down
        onRepeat: quantityInputField.text = (quantityInputField.value + 1).toString()
      }
    }

    UiStyle.TinyButton {
      text: i18n.t("All")
      onClicked: quantityInputField.text = maxQuantity
    }
  }

  onAccepted: {
    pickedValue(quantityInputField.value);
    quantityInputField.text = quantityInputField.value = defaultValue;
  }
}
