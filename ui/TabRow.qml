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
      background: UiStyle.Tab {}
      contentItem: Text {
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: parent.text
        font: parent.font
        color: "white"
      }
      onClicked: currentTab = tabs[index]
    }
  }
}
