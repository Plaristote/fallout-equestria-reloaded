import QtQuick 2.15
import QtQuick.Controls 2.15
import "../assets/ui" as UiStyle
import Game 1.0

Row {
  id: root
  property var tabs: []
  property var labels: tabs
  property string currentTab

  Repeater {
    model: tabs.length
    delegate: Button {
      text: labels[index]
      font.bold: currentTab === tabs[index]
      font.family: application.titleFontName
      font.pointSize: application.titleFont.pointSize
      background: UiStyle.Tab {}
      contentItem: Text {
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: parent.text
        font: parent.font
        leftPadding: 10
        rightPadding: 10
        color: "white"
      }
      onClicked: {
        currentTab = tabs[index]
        if (typeof soundManager != "undefined")
          soundManager.play("ui/tab-change");
      }
      onHoveredChanged: {
        mouseCursor.setCurrentPointer(hovered ? MouseCursor.ActivePointer : MouseCursor.NormalPointer)
      }
    }
  }
}
