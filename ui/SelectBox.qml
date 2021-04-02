import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

ComboBox {
  id: control
  implicitHeight: 40
  contentItem: Text {
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
    text: control.currentText
    font.family: application.titleFontName
    font.pixelSize: 16
    color: "white"
  }
  background: UiStyle.Label {
    style: parent.focus ? "dark" : "base"
  }
}
