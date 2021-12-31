import QtQuick 2.15

Row {
  property alias text: label.text
  property alias icon: iconDisplay.name
  property int size: 16

  spacing: 5
  Icon {
    id: iconDisplay
    width: size; height: size
    anchors.verticalCenter: parent.verticalCenter
  }
  TitleText {
    id: label
    font.pixelSize: size
    anchors.verticalCenter: parent.verticalCenter
  }
}
