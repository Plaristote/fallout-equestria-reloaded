import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle
import "../../ui"

SideBar {
  id: root
  property QtObject character
  property QtObject target
  property var skills: ["sneak", "steal", "explosives", "medicine", "lockpick", "repair", "science"]

  signal pickedSkill(string skillName)

  title: i18n.t("cmap.skills")
  width: 160 + 75 + 35
  content: ColumnLayout {
    spacing: 5

    Repeater {
      model: root.skills
      delegate: Row {
        spacing: 5
        Layout.fillWidth: true

        Button {
          background: UiStyle.Label { style: parent.down ? "dark" : "base" }
          text: i18n.t(`cmap.${root.skills[index]}`)
          width: parent.width - 80
          height: 40
          contentItem: UiStyle.TitleText {
            anchors.centerIn: parent
            text: parent.text
            color: "white"
            outline: "black"
            font.pointSize: application.titleFont.pointSize
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
