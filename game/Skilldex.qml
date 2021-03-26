import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  id: root
  property QtObject character
  property var skills: ["sneak", "steal", "medicine", "lockpick", "repair", "science"]

  signal pickedSkill(string skillName)

  background: UiStyle.Pane {}

  ColumnLayout {
    spacing: 5

    Repeater {
      model: root.skills
      delegate: Row {
        Button {
          background: UiStyle.Label { style: parent.down ? "dark" : "base" }
          text: root.skills[index]
          width: 160
          height: 30
          contentItem: Label {
            anchors.centerIn: parent
            text: parent.text
            color: "white"
          }
          onClicked: root.pickedSkill(root.skills[index])
        }

        Label {
          width: 50
          height: 30
          background: UiStyle.TerminalPane{}
          text: `${root.character.statistics[root.skills[index]]} %`
          color: "white"
          verticalAlignment: Text.AlignVCenter
          horizontalAlignment: Text.AlignHCenter
        }
      }
    }
  }
}
