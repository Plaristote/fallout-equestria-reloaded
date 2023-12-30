import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../ui"

RowLayout {
  id: root
  property string translationKey
  property string prefix
  property string translationFile
  property string lastModifiedText

  signal updated()
  signal requireKeyChange(string newKey)

  function scheduleUpdate() {
    if (!updateTimer.running) {
      lastModifiedText = textInput.text;
      updateTimer.start();
    }
  }

  function update() {
    const key = prefix + '.' + translationKey;

    if (i18n.t(key) !== textInput.text) {
      scriptController.setTranslation(key, textInput.text, translationFile);
      updated();
    }
  }

  Timer {
    id: updateTimer
    interval: 750
    onTriggered: {
      if (textInput.text == lastModifiedText)
        root.update();
      else
        root.scheduleUpdate();
    }
  }

  TerminalField {
    id: textInput
    Layout.fillWidth: true
    implicitHeight: focus ? 120 : 40
    wrapMode: Text.WordWrap
    text: i18n.t(prefix + '.' + translationKey)
    onTextChanged: root.scheduleUpdate()

    Behavior on implicitHeight {
      NumberAnimation { duration: 200 }
    }
  }

  TerminalButton {
    id: button
    text: translationKey
    onClicked: keyPickerDialog.open()
    clip: true
    implicitWidth: 150
    UiStyle.TerminalToolTip { target: button }
    TextPromptDialog {
      id: keyPickerDialog
      parent: Overlay.overlay
      anchors.centerIn: parent
      title: `${i18n.t("game-editor.translation-key")}: ${prefix}`
      value: root.translationKey
      onAccepted: requireKeyChange(value)
    }
  }
}
