import QtQuick 2.15

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

  implicitWidth: image.implicitWidth
  implicitHeight: image.height + 20

  Column {
    anchors.left: parent.left
    anchors.right: parent.right

    Text {
      text:  root.text
      color: root.color
      anchors.horizontalCenter: image.horizontalCenter
      font.family: application.titleFontName
      font.pixelSize: 20
    }

    Image {
      id: image
      source: buttonSource(down, hovered)
    }
  }

  MouseArea {
    id: mouseArea
    anchors.fill: parent
    onClicked: root.clicked()
  }
}
