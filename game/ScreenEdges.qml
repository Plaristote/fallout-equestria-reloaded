import QtQuick 2.0

Item {
  anchors.fill: parent

  property int scrollBorderSize: 40
  property int scrollSpeed: 20
  property bool enabled: true

  signal moveTop();
  signal moveLeft();
  signal moveRight();
  signal moveBottom();

  MouseArea {
    id: topLeftCorner
    anchors { top: parent.top; left: parent.left }
    height: scrollBorderSize
    width: scrollBorderSize
    hoverEnabled: parent.enabled
    propagateComposedEvents: true
    Timer {
      interval: 100
      repeat: true
      running: parent.containsMouse
      onTriggered: { moveTop(); moveLeft() }
    }
  }

  MouseArea {
    id: topRightCorner
    anchors { top: parent.top; right: parent.right }
    height: scrollBorderSize
    width: scrollBorderSize
    hoverEnabled: parent.enabled
    propagateComposedEvents: true
    Timer {
      interval: 100
      repeat: true
      running: parent.containsMouse
      onTriggered: { moveTop(); moveRight() }
    }
  }

  MouseArea {
    id: bottomLeftCorner
    anchors { bottom: parent.bottom; right: parent.left }
    height: scrollBorderSize
    width: scrollBorderSize
    hoverEnabled: parent.enabled
    propagateComposedEvents: true
    Timer {
      interval: 100
      repeat: true
      running: parent.containsMouse
      onTriggered: { moveBottom(); moveLeft() }
    }
  }

  MouseArea {
    id: bottomRightCorner
    anchors { bottom: parent.bottom; right: parent.right }
    height: scrollBorderSize
    width: scrollBorderSize
    hoverEnabled: parent.enabled
    propagateComposedEvents: true
    Timer {
      interval: 100
      repeat: true
      running: parent.containsMouse
      onTriggered: { moveBottom(); moveRight() }
    }
  }

  MouseArea {
    anchors { top: parent.top; left: topLeftCorner.right; right: topRightCorner.left; }
    height: scrollBorderSize
    hoverEnabled: parent.enabled
    propagateComposedEvents: true
    Timer {
      interval: 100
      repeat: true
      running: parent.containsMouse
      onTriggered: moveTop()
    }
  }

  MouseArea {
    anchors { top: topLeftCorner.bottom; left: parent.left; bottom: bottomLeftCorner.top }
    width: scrollBorderSize
    hoverEnabled: parent.enabled
    propagateComposedEvents: true
    Timer {
      interval: 100
      repeat: true
      running: parent.containsMouse
      onTriggered: moveLeft()
    }
  }

  MouseArea {
    anchors { left: bottomLeftCorner.right; bottom: parent.bottom; right: bottomRightCorner.left; }
    height: scrollBorderSize
    hoverEnabled: parent.enabled
    propagateComposedEvents: true
    Timer {
      interval: 100
      repeat: true
      running: parent.containsMouse
      onTriggered: moveBottom()
    }
  }

  MouseArea {
    anchors { top: topRightCorner.bottom; right: parent.right; bottom: bottomRightCorner.top; }
    width: scrollBorderSize
    hoverEnabled: parent.enabled
    propagateComposedEvents: true
    Timer {
      interval: 100
      repeat: true
      running: parent.containsMouse
      onTriggered: moveRight()
    }
  }
}
