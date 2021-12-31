import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  id: root
  property QtObject character
  property QtObject target
  property var skills: ["sneak", "steal", "explosives", "medicine", "lockpick", "repair", "science"]

  signal pickedSkill(string skillName)

  background: UiStyle.Pane {}
  width: 160 + 75 + 35

  ColumnLayout {
    spacing: 5

    Repeater {
      model: root.skills
      delegate: Row {
        spacing: 5

        Button {
          background: UiStyle.Label { style: parent.down ? "dark" : "base" }
          text: i18n.t(`cmap.${root.skills[index]}`)
          width: 160
          height: 40
          contentItem: UiStyle.TitleText {
            anchors.centerIn: parent
            text: parent.text
            color: "white"
            outline: "black"
            font.pointSize: 18
          }
          onClicked: root.pickedSkill(root.skills[index])
        }

        Label {
          width: 75
          height: 40
          background: UiStyle.TerminalPane{}
          text: `${root.character.statistics[root.skills[index]]} %`
          font.family: application.consoleFont.name
          color: "white"
          verticalAlignment: Text.AlignVCenter
          horizontalAlignment: Text.AlignHCenter
        }
      }
    }
  }
}
