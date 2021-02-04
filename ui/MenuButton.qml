import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Button {
  font.bold: true
  contentItem: Text {
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    leftPadding: 25
    text: parent.text
    font: parent.font
    color: "white"
  }
  background: UiStyle.MenuButton {}
}
