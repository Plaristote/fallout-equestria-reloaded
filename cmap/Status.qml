import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject characterSheet
  property var injuries: [
    "poisoned", "radiated", "eye-damage", "crippled-front-left-leg", "crippled-front-right-leg", "crippled-rear-left-leg", "crippled-rear-right-leg"
  ]

  background: UiStyle.TerminalPane {}

  ColumnLayout {
    RowLayout {
      Label {
        text: qsTr("Hit Points")
        Layout.alignment: Qt.AlignLeft
        color: "white"
      }
      Label {
        text: "50/50"
        Layout.alignment: Qt.AlignRight
        color: "white"
      }
    }

    Repeater {
      model: injuries
      delegate: Label {
        text: qsTr(injuries[index])
        color: false ? "white" : "gray"
      }
    }
  }
}
