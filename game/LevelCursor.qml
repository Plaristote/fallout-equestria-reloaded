import QtQuick 2.15

Item {
  property int mouseMode
  property int mouseX
  property int mouseY
  anchors.fill: parent

  Image {
    visible: mouseMode === 0
    source: "qrc:/assets/ui/cursors/move-tile.png"
    height: 36
    width: 72
    x: mouseX - 18
    y: mouseY - 36
  }

  Image {
    visible: mouseMode === 1
    source: "qrc:/assets/ui/cursors/interaction.png"
    height: 50
    width: 64
    x: mouseX
    y: mouseY
  }

  Image {
    visible: mouseMode === 2
    source: "qrc:/assets/ui/cursors/target.png"
    height: 50
    width: 50
    x: mouseX - 20
    y: mouseY - 20
  }
}
