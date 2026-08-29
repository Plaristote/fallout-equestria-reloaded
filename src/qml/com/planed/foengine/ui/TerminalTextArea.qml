import QtQuick 2.15
import QtQuick.Controls 2.15

ScrollView {
  property alias wrapMode: field.wrapMode
  property alias text: field.text
  property alias textFormat: field.textFormat

  TextArea {
    id: field
    color: "green"
    placeholderTextColor: "gray"
    textFormat: TextEdit.PlainText
    font.family: application.consoleFontName
    background: Rectangle {
      color: "transparent";
      border.color: field.focus ? "white" : "green";
      border.width: 1
    }
  }
}
