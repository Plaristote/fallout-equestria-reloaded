import QtQuick 2.15
import QtQuick.Controls 2.15

ComboBox {
  id: root
  contentItem: Text {
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
    text: root.currentText
    color: "green"
    padding: 10
    font.family: application.consoleFontName
  }
  background: Rectangle {
    border.color: root.activeFocus ? "white" : "green"
    border.width: 1
    color: "transparent"
  }
}
