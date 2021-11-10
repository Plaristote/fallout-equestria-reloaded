import QtQuick 2.15

Image {
  source: "qrc:/assets/backgrounds/loading-screen.png"
  anchors.fill: parent
  fillMode: Image.PreserveAspectCrop

  Rectangle {
    anchors.centerIn: parent
    width: parent.width
    height: 100
    color: Qt.rgba(0, 0, 0, 0.5)

    Text {
      id: loadingLabel
      anchors.centerIn: parent
      color: "white"
      text: qsTr("Loading")
    }

    Text {
      y: loadingLabel.y
      anchors.left: loadingLabel.right
      text: "..."
      color: "white"
      Timer {
        running: root.visible
        repeat: true
        interval: 300
        onTriggered: parent.text = parent.text.length >= 3 ? '.' : parent.text + '.'
      }
    }
  }
}
