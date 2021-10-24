import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {
  property QtObject levelController

  visible: levelController && levelController.debugMode
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
}
