import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject characterSheet

  background: UiStyle.TerminalPane {}

  ColumnLayout {
    anchors.left: parent.left
    anchors.right: parent.right

    Text {
      Layout.alignment: Qt.AlignCenter
      text: qsTr("Level") + ": 2"
      color: "white"
    }
    Text {
      Layout.alignment: Qt.AlignCenter
      text: qsTr("Experience") + ": 1301"
      color: "white"
    }
    Text {
      Layout.alignment: Qt.AlignCenter
      text: qsTr("Next level") + ": 3000"
      color: "white"
    }
  }
}
