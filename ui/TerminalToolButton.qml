import QtQuick 2.15
import "../assets/ui" as UiStyle

TerminalButton {
  id: root
  property string iconName
  property bool hideText: false

  padding: 0
  contentItem: Row {
    height: root.height

    Item { width: 5; height: 5 }

    UiStyle.Icon {
      anchors.verticalCenter: parent.verticalCenter
      name: iconName
      height: root.height - 5
      width: height
    }

    Item { width: root.text.length > 0 ? 5 : 0; height: 5 }

    Text {
      anchors.verticalCenter: parent.verticalCenter
      text: root.text
      color: "white"
      font.family: root.font.family
      visible: root.text.length > 0 && !hideText
    }

    Item { width: 5; height: 5 }
  }
}
