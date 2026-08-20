import QtQuick 2.15
import QtQuick.Controls 2.15
import "../../ui"

Column {
  id: root
  spacing: 6
  property QtObject gameController
  readonly property QtObject timeManager: gameController.timeManager
  property string mode: "until"
  property int pendingSeconds: 0
  property int remainingSeconds: 0
  property bool waiting: false
  property bool waitingForHeal: false
  property int selectedDayOffset: 0
  property var dayLabels: []

  Component.onCompleted: {
    refreshDayLabels();
    untilHourBox.value = timeManager.hour;
    untilMinuteBox.value = timeManager.minute;
    recomputeUntilSeconds();
  }
  Component.onDestruction: gameController.player.setVariable("resting", false)

  Connections {
    target: timeManager
    function onDateChanged() { refreshDayLabels(); }
  }

  function refreshDayLabels() {
    var labels = [];
    for (var i = 0; i < 7; i++) {
      if (i === 0)
        labels.push(i18n.t("pipboy.clock-today"));
      else if (i === 1)
        labels.push(i18n.t("pipboy.clock-tomorrow"));
      else
        labels.push(timeManager.toStringAfter(i * 86400, "%w"));
    }
    dayLabels = labels;
  }

  function recomputeUntilSeconds() {
    const currentSecondsInDay = root.timeManager.hour * 3600 + root.timeManager.minute * 60 + root.timeManager.second;
    const targetSecondsInDay = untilHourBox.value * 3600 + untilMinuteBox.value * 60;
    const diff = targetSecondsInDay - currentSecondsInDay;
    const dayOffset = root.selectedDayOffset;

    if (dayOffset === 0 && diff <= 0)
      pendingSeconds = -1;
    else {
      pendingSeconds = root.timeManager.secondsUntilTime({
        days: dayOffset,
        hour: untilHourBox.value,
        minute: untilMinuteBox.value
      });
    }
  }

  function recomputeDurationSeconds() {
    pendingSeconds = (durationDaysBox.value * 1440 + durationHoursBox.value * 60 + durationMinutesBox.value) * 60;
  }

  function setMode(newMode) {
    mode = newMode;
    if (mode === "until")
      recomputeUntilSeconds();
    else
      recomputeDurationSeconds();
  }

  function isFullyHealed() {
    return gameController.player.statistics.hitPoints >= gameController.player.statistics.maxHitPoints;
  }

  function startWaiting(seconds) {
    if (seconds <= 0)
      return;
    gameController.player.setVariable("resting", true);
    remainingSeconds = seconds;
    waiting = true;
    waitTimer.running = true;
  }

  function startWaitingForHeal() {
    if (isFullyHealed())
      return;
    gameController.player.setVariable("resting", true);
    waitingForHeal = true;
    waiting = true;
    waitHealTimer.running = true;
  }

  function stopWaiting() {
    waiting = false;
    waitingForHeal = false;
    waitTimer.running = false;
    waitHealTimer.running = false;
    remainingSeconds = 0;
    gameController.player.setVariable("resting", false);
    if (mode === "until")
      recomputeUntilSeconds();
    else
      recomputeDurationSeconds();
  }

  function formatRemaining(seconds) {
    var totalMinutes = Math.max(0, Math.ceil(seconds / 60));
    var days = Math.floor(totalMinutes / 1440);
    var hours = Math.floor((totalMinutes % 1440) / 60);
    var minutes = totalMinutes % 60;
    var text = "";
    if (days > 0)
      text += days + "d ";
    if (days > 0 || hours > 0)
      text += hours + "h ";
    text += minutes + "m";
    return text;
  }

  function triggerQuickWait(seconds) {
    if (seconds === 0)
      startWaitingForHeal();
    else
      startWaiting(seconds);
  }

  function updateWaitingHint() {
    const chars = ['․', '‥', '…'];
    const currentIndex = chars.indexOf(waitingHint.text);
    waitingHint.text = currentIndex + 1 >= chars.length ? chars[0] : chars[currentIndex + 1];
  }

  Timer {
    id: waitTimer
    interval: 100
    repeat: true
    onTriggered: {
      if (root.remainingSeconds <= 0) {
        root.stopWaiting();
        return;
      }
      var remainingMinutes = Math.ceil(root.remainingSeconds / 60);
      var stepMinutes = 1;
      if (remainingMinutes > 360)
        stepMinutes = 68;
      else if (remainingMinutes > 60)
        stepMinutes = 12;
      else if (remainingMinutes > 15)
        stepMinutes = 3;
      stepMinutes = Math.min(stepMinutes, remainingMinutes);

      gameController.advanceTime(stepMinutes);
      root.remainingSeconds -= stepMinutes * 60;
      updateWaitingHint();
    }
  }

  Timer {
    id: waitHealTimer
    interval: 100
    repeat: true
    onTriggered: {
      gameController.advanceTime(60);
      updateWaitingHint();
      if (root.isFullyHealed())
        root.stopWaiting();
    }
  }

  TerminalLabel {
    anchors.horizontalCenter: parent.horizontalCenter
    text: i18n.t("pipboy.clock-title")
    color: "white"
    font.pointSize: application.consoleFont.bigSize
    Text {
      id: waitingHint
      visible: root.waiting
      anchors.left: parent.right
      text: "…"
      color: "white"
    }
  }

  TerminalLabel {
    anchors.horizontalCenter: parent.horizontalCenter
    font.pointSize: application.consoleFont.normalSize
    text: {
      var seconds = root.waiting ? root.remainingSeconds : root.pendingSeconds;
      if (root.waitingForHeal)
        return i18n.t("pipboy.clock-wait-heal");
      if (seconds > 0)
        return i18n.t("pipboy.clock-wait-ends", {time: timeManager.toStringAfter(seconds, "%w %d/%M %h:%m")});
      return i18n.t("pipboy.clock-wait-invalid");
    }
  }

  TerminalLabel {
    visible: root.waiting && !root.waitingForHeal
    anchors.horizontalCenter: parent.horizontalCenter
    font.pointSize: application.consoleFont.normalSize
    text: i18n.t("pipboy.clock-wait-remaining", {time: root.formatRemaining(root.remainingSeconds)})
  }

  Row {
    spacing: 8
    visible: !root.waiting

    TerminalButton {
      text: i18n.t("pipboy.clock-mode-until")
      backgroundColor: root.mode === "until" ? "green" : (down ? "green" : "transparent")
      textColor: root.mode === "until" ? "black" : "white"
      leftPadding: 12
      rightPadding: 12
      topPadding: 4
      bottomPadding: 4
      onClicked: root.setMode("until")
    }
    TerminalButton {
      text: i18n.t("pipboy.clock-mode-duration")
      backgroundColor: root.mode === "duration" ? "green" : (down ? "green" : "transparent")
      textColor: root.mode === "duration" ? "black" : "white"
      leftPadding: 12
      rightPadding: 12
      topPadding: 4
      bottomPadding: 4
      onClicked: root.setMode("duration")
    }
  }

  // BEGIN Until form
  Row {
    visible: !root.waiting && root.mode === "until"
    spacing: 8

    TerminalComboBox {
      id: dayCombo
      implicitWidth: 200
      implicitHeight: untilHourBox.implicitHeight
      anchors.verticalCenter: parent.verticalCenter
      model: root.dayLabels
      currentIndex: root.selectedDayOffset
      onActivated: (index) => {
        root.selectedDayOffset = index;
        root.recomputeUntilSeconds();
      }
    }

    TerminalSpinBox {
      id: untilHourBox
      from: 0
      to: 23
      leftPadding: 60
      rightPadding: 60
      onValueModified: root.recomputeUntilSeconds()
    }
    TerminalLabel { text: "h"; anchors.verticalCenter: parent.verticalCenter }

    TerminalSpinBox {
      id: untilMinuteBox
      from: 0
      to: 59
      leftPadding: 60
      rightPadding: 60
      onValueModified: root.recomputeUntilSeconds()
    }
    TerminalLabel { text: "m"; anchors.verticalCenter: parent.verticalCenter }
  }
  // END Until form

  // BEGIN Duration form
  Row {
    visible: !root.waiting && root.mode === "duration"
    spacing: 8

    TerminalSpinBox {
      id: durationDaysBox
      from: 0
      to: 30
      value: 0
      leftPadding: 60
      rightPadding: 60
      onValueModified: root.recomputeDurationSeconds()
    }
    TerminalLabel { text: i18n.t("pipboy.clock-unit-days"); anchors.verticalCenter: parent.verticalCenter }

    TerminalSpinBox {
      id: durationHoursBox
      from: 0
      to: 23
      value: 0
      leftPadding: 60
      rightPadding: 60
      onValueModified: root.recomputeDurationSeconds()
    }
    TerminalLabel { text: i18n.t("pipboy.clock-unit-hours"); anchors.verticalCenter: parent.verticalCenter }

    TerminalSpinBox {
      id: durationMinutesBox
      from: 0
      to: 59
      value: 0
      leftPadding: 60
      rightPadding: 60
      onValueModified: root.recomputeDurationSeconds()
    }
    TerminalLabel { text: i18n.t("pipboy.clock-unit-minutes"); anchors.verticalCenter: parent.verticalCenter }
  }
  // END Duration form

  Row {
    spacing: 16

    TerminalButton {
      visible: !root.waiting
      enabled: root.pendingSeconds > 0
      text: i18n.t("pipboy.clock-wait-confirm")
      leftPadding: 12
      rightPadding: 12
      topPadding: 4
      bottomPadding: 4
      onClicked: root.startWaiting(root.pendingSeconds)
    }

    TerminalButton {
      visible: root.waiting
      text: i18n.t("pipboy.clock-wait-cancel")
      leftPadding: 12
      rightPadding: 12
      topPadding: 4
      bottomPadding: 4
      onClicked: root.stopWaiting()
    }
  }

  Item {
    width: 20
    height: 20
  }

  Column {
    visible: !root.waiting
    spacing: 8

    Repeater {
      model: [
        {label: i18n.t("pipboy.clock-wait-hour", {time: 1}), seconds: 3600},
        {label: i18n.t("pipboy.clock-wait-hour", {time: 2}), seconds: 7200},
        {label: i18n.t("pipboy.clock-wait-hour", {time: 12}), seconds: 7200*6},
        {label: i18n.t("pipboy.clock-wait-day",  {time: 1}), seconds: 86400},
        {label: i18n.t("pipboy.clock-wait-day",  {time: 2}), seconds: 86400*2},
        {label: i18n.t("pipboy.clock-wait-day",  {time: 7}), seconds: 86400*7},
        {label: i18n.t("pipboy.clock-wait-heal"), seconds: 0}
      ]
      delegate: Button {
        required property var modelData
        text: modelData.label
        leftPadding: 8
        rightPadding: 8
        topPadding: 4
        bottomPadding: 4
        onClicked: root.triggerQuickWait(modelData.seconds)
        background: Rectangle { color: "transparent" }
        contentItem: Text {
          color: "yellow"
          text: modelData.label
          font.family: application.consoleFont.name
          font.pointSize: application.consoleFont.bigSize
          font.underline: parent.hovered
        }
      }
    }
  }

  DynamicObjectMetrics {
    levelController: gameController.level
  }
}
