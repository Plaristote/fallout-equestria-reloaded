import QtQuick 2.0
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../assets/ui" as UiStyle

Pane {
  id: root
  property string title
  property var savedGameList: gameManager.getSavedGames()
  property int selectedIndex: 0
  property alias slots: slotList.children
  property alias controls: controlsRow.children
  background: UiStyle.Pane {}

  Text {
    id: viewTitle
    text: title
    color: "white"
    font.pointSize: 25
    anchors.topMargin: 10
    anchors.leftMargin: 10
    anchors.top: parent.top
    anchors.left: parent.left
  }

  Pane {
    id: slotListPane
    background: UiStyle.TerminalPane {}
    anchors.bottom: controlsRow.top;
    anchors.left: parent.left
    anchors.top: viewTitle.bottom
    width: parent.width * 0.7

    Flickable {
      contentHeight: slotList.height
      anchors.fill: parent
      clip: true

      ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

      ColumnLayout {
        id: slotList
        width: parent.width - 15
      }
    }
  }

  Pane {
    id: savePreview
    background: UiStyle.Pane {}
    anchors.bottom: controlsRow.top
    anchors.left: slotListPane.right
    anchors.top: viewTitle.bottom
    anchors.right: parent.right
  }

  Row {
    id: controlsRow
    anchors { bottom: parent.bottom; right: parent.right; }
  }
}
