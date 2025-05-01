import QtQuick 2.15

Item {
  id: combatSpeedControllerItem
  implicitWidth: textDisplay.width
  implicitHeight: textDisplay.height
  states: [
    State {
      name: "active"
      when: activityTimer.running
      PropertyChanges { target: combatSpeedControllerItem; opacity: 1 }
    },
    State {
      name: "inactive"
      when: !activityTimer.running
      PropertyChanges { target: combatSpeedControllerItem; opacity: 0 }
    }
  ]

  transitions: Transition {
    NumberAnimation { property: "opacity"; duration: 150; easing.type: Easing.InOutQuad }
  }

  Timer {
    id: activityTimer
    repeat: false
    interval: 1500
  }

  Text {
    id: textDisplay
    color: "white"
    font.family: application.titleFontName
    font.pointSize: application.titleFont.bigSize
    text: `Speed: ${gameManager.combatSpeedOption}`
  }

  Shortcut {
    sequence: "+"
    onActivated: {
      if (gameManager.combatSpeedOption < 10)
        gameManager.combatSpeedOption++;
      activityTimer.restart();
    }
  }

  Shortcut {
    sequence: "-"
    onActivated: {
      if (gameManager.combatSpeedOption > 0)
        gameManager.combatSpeedOption--;
      activityTimer.restart();
    }
  }
}

