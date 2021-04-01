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
    inputField.forceActiveFocus();
  }

  function validate() {
    return value.length > 0;
  }

  GridLayout {
    columns: 2
    anchors.fill: parent

    Text {
      text: fieldName
      color: "white"
      font.family: application.titleFontName
      font.pixelSize: 16
    }

    TextField {
      id: inputField
      Layout.fillWidth: true
      color: "white"
      background: UiStyle.Label {
        style: parent.focus ? "dark" : "base"
      }
      onAccepted: if (validate()) { root.accept(); }
    }

    Text {
      Layout.columnSpan: 2
      color: "red"
      font.family: application.titleFontName
      font.pixelSize: 17
      text: validationError
      visible: validationError && validationError.length > 0
    }
  }
}
