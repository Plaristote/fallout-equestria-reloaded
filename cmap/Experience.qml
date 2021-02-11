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
      text: qsTr("Level") + ": " + characterSheet.level
      color: "white"
    }
    Text {
      Layout.alignment: Qt.AlignCenter
      text: qsTr("Experience") + ": " + characterSheet.experience
      color: "white"
    }
    Text {
      Layout.alignment: Qt.AlignCenter
      text: qsTr("Next level") + ": " + characterSheet.xpNextLevel
      color: "white"
    }
  }
}
