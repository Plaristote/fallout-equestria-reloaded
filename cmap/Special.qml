import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Repeater {
  readonly property var list: [
    "STR", "PER", "END", "CHA", "INT", "AGI", "LUC"
  ]
  property QtObject characterSheet
  property bool editable: false

  model: list
  delegate: RowLayout {
    Label {
      Layout.minimumWidth: 100
      text: list[index]
      color: "white"
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      background: UiStyle.Pane {}
    }
    Label {
      Layout.minimumWidth: 50
      text: "5"
      color: "white"
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      padding: 5
      background: UiStyle.TerminalPane {}
    }
    Label {
      Layout.minimumWidth: 100
      text: "Average"
      color: "white"
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      padding: 5
      background: UiStyle.TerminalPane {}
    }
    UiStyle.TinyButton {
      visible: editable
      text: "+"
    }
    UiStyle.TinyButton {
      visible: editable
      text: "-"
    }
  }
}
