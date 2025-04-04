import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property int displayValue: 0
  property int source: 0

  background: UiStyle.TerminalPane {}
  width: 60
  height: 40

  Text {
    anchors.centerIn: parent
    text: displayValue.toString()
    color: "white"
    font.family: application.titleFontName
    font.pixelSize: 16
  }

  Timer {
    interval: 100
    running: displayValue !== source
    onTriggered: {
      if (displayValue > source)
        displayValue--;
      else
        displayValue++;
    }
  }
}
