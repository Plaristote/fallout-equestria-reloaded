import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

Pane {
  id: root
  property int maxHeight
  property var spells: character.statistics.spells
  property string title
  property alias content: contentItem.sourceComponent

  background: UiStyle.Pane {}
  height: Math.min(maxHeight, titlePane.height + contentItem.height + 40)
  padding: 0

  Pane {
    id: titlePane
    background: UiStyle.Pane {}
    anchors { top: parent.top; left: parent.left; right: parent.right }
    anchors.margins: 0
    visible: title.length > 0
    height: visible ? titleText.height + 30 : 0

    UiStyle.TitleText {
      id: titleText
      text: root.title
      color: "white"
      font.pointSize: application.titleFont.pointSize
      anchors.top: parent.top
      anchors.horizontalCenter: parent.horizontalCenter
    }
  }

  CustomFlickable {
    id: scrollZone
    clip: true
    contentHeight: contentItem.height
    anchors {
      top: titlePane.bottom
      bottom: parent.bottom
      left: parent.left
      right: parent.right
      margins: 5
    }

    ScrollBar.vertical: UiStyle.HudScrollbar {
      id: scrollBar
      orientation: Qt.Vertical
      visible: contentItem.height > scrollZone.height
    }

    Loader {
      id: contentItem
      anchors { left: parent.left; right: parent.right }
      anchors.rightMargin: scrollBar.visible ? scrollBar.width : 0
    }
  }
}
