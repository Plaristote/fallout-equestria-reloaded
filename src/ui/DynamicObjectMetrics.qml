import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
  property QtObject levelController
  property bool hidden: false

  visible: levelController && levelController.debugMode && !hidden
  height: 600
  width: Math.min(400, objectMetricsText.width)
  color: Qt.rgba(255, 255, 255, 0.7)
  border.color: "green"
  onVisibleChanged: {
    levelController.resetMetrics();
    refresh();
  }

  function refresh() {
    objectMetricsText.text = levelController.metricsHtml();
    //levelController.resetMetrics();
  }

  Flickable {
    anchors.fill: parent
    contentHeight: objectMetricsText.height
    contentWidth:  objectMetricsText.width
    clip: true
    Text {
      id: objectMetricsText
      textFormat: Text.RichText
    }
  }

  Timer {
    running: parent.visible
    repeat: true
    interval: 1000
    onTriggered: refresh()
  }

  Button {
    anchors { top: parent.top; right: parent.right }
    text: hidden ? "o" : "x"
    onClicked: hidden = !hidden
    width: 20
    height: 20
    opacity: 0.8
  }
}
