import QtQuick 2.12
import QtQuick.Controls 2.12

Flickable {
  property int maxContentY: contentHeight - height
  property int maxContentX: contentWidth  - width

  function scrollTo(x, y) {
    contentX = Math.min(maxContentX, Math.max(0, x));
    contentY = Math.min(maxContentY, Math.max(0, y));
  }

  function centerOn(position) {
    scrollTo(position.x * caseSize - width / 2, position.y * caseSize - height / 2);
  }

  Connections {
    target: gamepad
    enabled: hoverArea.containsMouse
    function onMoveCameraXAxis(movement) { scrollTo(flickable.contentX + movement / 3, flickable.contentY); }
    function onMoveCameraYAxis(movement) { scrollTo(flickable.contentX, flickable.contentY + movement / 3); }
  }

  MouseArea {
    id: hoverArea
    anchors.fill: parent
    hoverEnabled: true
  }
}
