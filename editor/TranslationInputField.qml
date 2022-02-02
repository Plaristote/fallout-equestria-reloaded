import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../ui"

RowLayout {
  id: root
  property string translationKey
  property string prefix

  signal updated()
  signal requireKeyChange(string newKey)

  function update() {
    const key = prefix + '.' + translationKey;

    if (i18n.t(key) !== textInput.text) {
      scriptController.setTranslation(key, textInput.text);
      updated();
    }
  }

  TerminalField {
    id: textInput
    Layout.fillWidth: true
    implicitHeight: focus ? 120 : 40
    wrapMode: Text.WordWrap
    text: i18n.t(prefix + '.' + translationKey)
    onTextChanged: root.update()

    Behavior on implicitHeight {
      NumberAnimation { duration: 200 }
    }
  }

  TerminalButton {
    text: translationKey
    onClicked: keyPickerDialog.open()
    TextPromptDialog {
      id: keyPickerDialog
      parent: Overlay.overlay
      anchors.centerIn: parent
      title: "Translation key: " + prefix
      value: root.translationKey
      onAccepted: requireKeyChange(value)
    }
  }
}
