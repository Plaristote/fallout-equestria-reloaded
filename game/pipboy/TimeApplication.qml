import QtQuick 2.15
import QtQuick.Controls 2.15

Column {
  property QtObject gameController
  readonly property QtObject timeManager: gameController.timeManager
  property int remainingMinutes: 0

  function getIntervals() {
    return [
      {label: "Wait 1 minute",   interval: 1},
      {label: "Wait 10 minutes", interval: 10},
      {label: "Wait 30 minutes", interval: 30},
      {label: "Wait 1 hour",     interval: 60},
      {label: "Wait 2 hours",    interval: 120},
      {label: "Wait 6 hours",    interval: 360},
      {label: "Wait 12 hours",   interval: 720},
      {label: "Wait 1 day",      interval: 1440},
      {label: "Wait until fully healed", interval: 0}
    ];
  }

  function triggerWaitFor(minutes) {
    if (minutes !== 0)
      remainingMinutes = minutes;
    else
      triggerWaitUntilHealed();
  }

  function triggerWaitUntilHealed() {

  }

  Timer {
    id: waitTimer
    interval: 100
    repeat: true
    running: remainingMinutes > 0
    onTriggered: {
      var value = 1;
      if (remainingMinutes > 15)
        value = 3;
      if (remainingMinutes > 60)
        value = 12;
      else if (remainingMinutes > 360)
        value = 68;
      gameController.advanceTime(value);
      remainingMinutes -= value;
      //
      if (waitingHint.text.length > 2)
        waitingHint.text = ".";
      else
        waitingHint.text += ".";
    }
  }

  Text {
    anchors.horizontalCenter: parent.horizontalCenter
    text: "Take a rest"
    color: "white"
    Text {
      id: waitingHint
      visible: waitTimer.running
      anchors.left: parent.right
      text: "..."
      color: "white"
    }
  }

  Repeater {
    model: getIntervals()
    delegate:  Button {
      property var intervalData: getIntervals()[index]

      hoverEnabled: true

      background: Rectangle {
        color: "transparent"
      }

      contentItem: Text {
        color: "yellow"
        text: intervalData.label
        font.family: application.consoleFont.name
        font.pointSize: application.consoleFont.bigSize
        font.underline: parent.hovered
      }

      onClicked: triggerWaitFor(intervalData.interval)
    }
  }
}
