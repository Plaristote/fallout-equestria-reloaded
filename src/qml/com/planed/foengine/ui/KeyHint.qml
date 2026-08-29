import QtQuick 2.15

Image {
  property string key
  source: "qrc:/assets/ui/key.png"

  Text {
    anchors.centerIn: parent
    text: key
  }
}
