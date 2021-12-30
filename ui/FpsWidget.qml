import QtQuick 2.15

Rectangle {
  id: fpsWidget
  property int frameCount: 0
  color: "transparent"
  height: fpsDisplay.height
  width:  fpsDisplay.width

  Connections {
    target: application
    enabled: fpsWidget.visible
    function onFrameSwapped() { fpsWidget.frameCount++; }
  }

  Timer {
    interval: 1000
    repeat: true
    running: true
    onTriggered: {
      fpsDisplay.text = fpsWidget.frameCount.toString() + "fps";
      frameCount = 0;
    }
  }

  Text {
    id: fpsDisplay
    color: "white"
    style: Text.Raised
    styleColor: "black"
    anchors.top: parent.top
    anchors.right: parent.right
    font.pixelSize: 25
  }
}
