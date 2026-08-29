import QtQuick 2.12
import QtQuick.Controls 2.12

Flickable {
  id: flickable
  property int maxContentY: contentHeight - height
  property int maxContentX: contentWidth  - width
  property bool activated: hoverArea.containsMouse

  onActivatedChanged: gamepad.hoverScrollable = activated

  function scrollTo(x, y) {
    if (maxContentX > 0)
      contentX = Math.min(maxContentX, Math.max(0, x));
    if (maxContentY > 0)
      contentY = Math.min(maxContentY, Math.max(0, y));
  }

  Connections {
    target: gamepad
    enabled: flickable.activated
    function onMoveCameraXAxis(movement) { scrollTo(flickable.contentX + movement, flickable.contentY); }
    function onMoveCameraYAxis(movement) { scrollTo(flickable.contentX, flickable.contentY + movement); }
  }

  MouseArea {
    id: hoverArea
    anchors.fill: parent
    hoverEnabled: true
  }
}
