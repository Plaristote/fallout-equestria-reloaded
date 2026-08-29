import QtQuick 2.15
import QtQuick.Controls 2.15
import Game 1.0

Button {
  id: button
  font.bold: true
  contentItem: Text {
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    text: button.text
    font: button.font
    color: "white"
  }
  background: Image {
    source: "qrc:/assets/ui/hud.png"
    sourceClipRect: !button.down ? Qt.rect(374, 32, 30, 30) : Qt.rect(404, 32, 30, 30)
  }
  onHoveredChanged: {
    mouseCursor.setCurrentPointer(hovered ? MouseCursor.ActivePointer : MouseCursor.NormalPointer)
  }
  Connections {
    target: button
    function onClicked() {
      if (typeof soundManager != "undefined")
        soundManager.play("ui/tiny-btn-click");
    }
  }
}
