import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../ui"

Column {
  property QtObject gameController
  readonly property QtObject timeManager: gameController.timeManager
  property int remainingMinutes: 0

  Component.onDestruction: gameController.player.setVariable("resting", false)

  function getIntervals() {
    return [
      {label: i18n.t("pipboy.clock-wait-minute", {time:  1}), interval: 1},
      {label: i18n.t("pipboy.clock-wait-minute", {time: 10}), interval: 10},
      {label: i18n.t("pipboy.clock-wait-minute", {time: 30}), interval: 30},
      {label: i18n.t("pipboy.clock-wait-hour",   {time:  1}), interval: 60},
      {label: i18n.t("pipboy.clock-wait-hour",   {time:  2}), interval: 120},
      {label: i18n.t("pipboy.clock-wait-hour",   {time:  6}), interval: 360},
      {label: i18n.t("pipboy.clock-wait-hour",   {time: 12}), interval: 720},
      {label: i18n.t("pipboy.clock-wait-day",    {time:  1}), interval: 1440},
      {label: i18n.t("pipboy.clock-wait-heal"), interval: 0}
    ];
  }

  function triggerWaitFor(minutes) {
    gameController.player.setVariable("resting", true);
    waitHealTimer.running = false;
    if (minutes !== 0)
      remainingMinutes = minutes;
    else
      triggerWaitUntilHealed();
  }

  function triggerWaitUntilHealed() {
    if (!isFullyHealed())
      waitHealTimer.running = true;
  }

  function isFullyHealed() {
    return gameController.player.statistics.hitPoints >= gameController.player.statistics.maxHitPoints;
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
      waitingHint.text = waitingHint.text.length > 2 ? "." : waitingHint.text + ".";
    }
  }

  Timer {
    id: waitHealTimer
    interval: 100
    repeat: true
    onTriggered: {
      gameController.advanceTime(60);
      if (isFullyHealed())
        running = false;
    }
  }

  Text {
    anchors.horizontalCenter: parent.horizontalCenter
    text: i18n.t("pipboy.clock-title")
    color: "white"
    font.family: application.consoleFont.name
    font.pointSize: application.consoleFont.bigSize
    Text {
      id: waitingHint
      visible: waitTimer.running || waitHealTimer.running
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

      onClicked: {
        triggerWaitFor(intervalData.interval);
        soundManager.play("ui/term-btn-click");
      }
    }
  }

  DynamicObjectMetrics {
    levelController: gameController.level
  }
}
