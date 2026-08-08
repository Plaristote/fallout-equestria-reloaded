import QtQuick 2.15
import QtQuick.Controls 2.15

SpinBox {
  id: spin
  implicitHeight: 25
  font.family: application.consoleFontName
  font.pointSize: application.consoleFont.normalSize

  contentItem: TextInput {
    text: spin.textFromValue(spin.value, spin.locale)
    font: spin.font
    color: "green"
    horizontalAlignment: Qt.AlignHCenter
    verticalAlignment: Qt.AlignVCenter
    readOnly: !spin.editable
    validator: spin.validator
    selectByMouse: true
    inputMethodHints: Qt.ImhDigitsOnly
  }

  up.indicator: Rectangle {
    x: spin.width - width
    height: spin.height
    implicitWidth: 20
    color: "transparent"
    border.color: spin.up.pressed ? "white" : "green"
    Text { anchors.centerIn: parent; text: "+"; color: "green"; font: spin.font }
  }
  down.indicator: Rectangle {
    x: 0
    height: spin.height
    implicitWidth: 20
    color: "transparent"
    border.color: spin.down.pressed ? "white" : "green"
    Text { anchors.centerIn: parent; text: "-"; color: "green"; font: spin.font }
  }
  background: Rectangle {
    color: "transparent"
    border.color: spin.activeFocus ? "white" : "green"
    border.width: 1
  }
}
