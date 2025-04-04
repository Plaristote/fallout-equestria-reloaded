import QtQuick 2.15

Timer {
  property int amountAdded:  0
  property int intervalBase: 400
  property int intervalMin:  65

  signal repeat()

  interval: amountAdded == 0 ? intervalBase : (intervalMin + Math.max(0, (intervalBase - intervalMin - amountAdded * 35)))
  repeat: true
  onRunningChanged: amountAdded = 0
  onTriggered: {
    amountAdded++;
    repeat();
  }
}
