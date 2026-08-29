import QtQuick 2.15
import QtQuick.Controls 2.15

TextField {
  id: field
  color: "green"
  placeholderTextColor: "gray"
  font.family: application.consoleFontName
  background: Rectangle {
    color: "transparent";
    border.color: field.focus ? "white" : "green";
    border.width: 1
  }
}
