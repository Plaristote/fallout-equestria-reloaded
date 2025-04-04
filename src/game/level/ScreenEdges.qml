import QtQuick 2.15

Item {
  id: root
  anchors.fill: parent

  property int scrollBorderSize: application.isMaximized ? 5 : 30
  property int scrollSpeed: 100
  property bool enabled: true

  signal moveTop();
  signal moveLeft();
  signal moveRight();
  signal moveBottom();

  Shortcut {
    sequence: "Up"
    enabled: root.enabled
    autoRepeat: true
    onActivated: moveTop()
  }

  Shortcut {
    sequence: "Left"
    enabled: root.enabled
    autoRepeat: true
    onActivated: moveLeft()
  }

  Shortcut {
    sequence: "Right"
    enabled: root.enabled
    autoRepeat: true
    onActivated: moveRight()
  }

  Shortcut {
    sequence: "Down"
    enabled: root.enabled
    autoRepeat: true
    onActivated: moveBottom()
  }

  Item {
    id: gamepadCamera
    property real cumulatedXMovement: 0
    property real cumulatedYMovement: 0
    property real threshold: 1

    Connections {
      target: gamepad
      enabled: root.enabled && !gamepad.hoverScrollable
      function onMoveCameraXAxis(value) {
        gamepadCamera.cumulatedXMovement += value;
        if (gamepadCamera.cumulatedXMovement > gamepadCamera.threshold)
          moveRight();
        else if (gamepadCamera.cumulatedXMovement <= -gamepadCamera.threshold)
          moveLeft();
        else
          return ;
        gamepadCamera.cumulatedXMovement = 0;
      }

      function onMoveCameraYAxis(value) {
        gamepadCamera.cumulatedYMovement += value;
        if (gamepadCamera.cumulatedYMovement >= gamepadCamera.threshold)
          moveBottom();
        else if (gamepadCamera.cumulatedYMovement <= -gamepadCamera.threshold)
          moveTop();
        else
          return ;
        gamepadCamera.cumulatedYMovement = 0;
      }
    }
  }

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
    anchors { bottom: parent.bottom; left: parent.left }
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
