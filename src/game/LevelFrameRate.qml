import QtQuick 2.15

Text {
  property var target
  property int myframeRate: 0

  text: "Frame Rate: " + myframeRate
  font.pointSize: 20
  styleColor: "white"; style: Text.Outline

  Timer {
    interval: 1000; repeat: true; running: true
    onTriggered: {
      parent.myframeRate = target.controller.frameCount;
      target.controller.frameCount = 0;
    }
  }
}
