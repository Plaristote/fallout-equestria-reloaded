import QtQuick 2.15
import QtQuick.Controls 2.15
import Game 1.0

ComboBox {
  id: root
  contentItem: Text {
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
    text: root.currentText
    color: "green"
    padding: 10
    font.family: application.consoleFontName
  }
  background: Rectangle {
    border.color: root.activeFocus ? "white" : "green"
    border.width: 1
    color: "transparent"
  }
  onHoveredChanged: {
    mouseCursor.setCurrentPointer(hovered ? MouseCursor.ActivePointer : MouseCursor.NormalPointer)
  }
  delegate: ItemDelegate {
    id: itemDelegate
    width: root.width
    onClicked: {
      if (typeof soundManager != "undefined")
        soundManager.play("ui/tab-change");
    }
    contentItem: Text {
      id: delegateText
      verticalAlignment: Text.AlignVCenter
      width: parent.width
      text: modelData.text || modelData
      color: itemDelegate.hovered ? (itemDelegate.down ? "black" : "white") : "green"
      padding: 5
      font.family: application.consoleFontName
      wrapMode: Text.WordWrap
    }
    background: Rectangle {
      color: itemDelegate.down && itemDelegate.hovered ? "green" : "black"
    }
  }
  popup.background: Rectangle {
    color: "black"
  }
}
