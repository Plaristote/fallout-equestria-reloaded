import QtQuick 2.15
import Game 1.0

Item {
  id: root
  property string text
  property color color: "white"
  property alias hovered: mouseArea.containsMouse
  property alias down: mouseArea.containsPress

  signal clicked()

  function buttonSource(down, hovered) {
    var source = "qrc:/assets/ui/button";

    if (down)
      source += "-pressed";
    return source + ".png";
  }

  implicitWidth: Math.max(image.implicitWidth, textMetrics.width)
  implicitHeight: buttonColumn.height

  onHoveredChanged: {
    mouseCursor.setCurrentPointer(hovered ? MouseCursor.ActivePointer : MouseCursor.NormalPointer)
  }

  TextMetrics {
    id: textMetrics
    font.family: application.titleFontName
    font.pointSize: application.titleFont.pointSize
    text: root.text
  }

  Column {
    id: buttonColumn
    anchors.left: parent.left
    anchors.right: parent.right

    Text {
      text:  root.text
      color: root.color
      anchors.horizontalCenter: image.horizontalCenter
      font.family: textMetrics.font.family
      font.pointSize: textMetrics.font.pointSize
    }

    Image {
      id: image
      source: buttonSource(down, hovered)
      anchors.horizontalCenter: parent.horizontalCenter
    }
  }

  MouseArea {
    id: mouseArea
    anchors.fill: parent
    onClicked: root.clicked()
    onContainsPressChanged: {
      if (typeof soundManager != "undefined")
        soundManager.play(`ui/push-button-${containsPress ? "in" : "out"}`)
    }
  }
}
