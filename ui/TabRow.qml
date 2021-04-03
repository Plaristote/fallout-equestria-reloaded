import QtQuick 2.15
import QtQuick.Controls 2.15
import "../assets/ui" as UiStyle

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
      font.pointSize: 16
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
      onClicked: currentTab = tabs[index]
    }
  }
}
