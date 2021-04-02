import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import "../assets/ui" as UiStyle

UiStyle.CustomDialog {
  id: root
  property string fieldName: "Name"
  property string validationError: ""
  property alias value: inputField.text
  modal: true
  standardButtons: Dialog.Ok | Dialog.Cancel

  onValidationErrorChanged: {
    inputField.forceActiveFocus();
  }

  onOpened: {
    value = "";
    validationError = "";
    inputField.forceActiveFocus();
  }

  function validate() {
    return value.length > 0;
  }

  GridLayout {
    columns: 2
    anchors.fill: parent

    CustomLabel {
      text: fieldName
    }

    CustomTextField {
      id: inputField
      Layout.fillWidth: true
      onAccepted: if (validate()) { root.accept(); }
    }

    CustomLabel {
      Layout.columnSpan: 2
      color: "red"
      font.pixelSize: 17
      text: validationError
      visible: validationError && validationError.length > 0
    }
  }
}
