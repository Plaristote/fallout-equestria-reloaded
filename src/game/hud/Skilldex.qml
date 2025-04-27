import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Game 1.0
import "qrc:/assets/ui" as UiStyle
import "../../ui"

SideBar {
  property QtObject character
  property QtObject target
  property var skills: ["sneak", "steal", "explosives", "medicine", "lockpick", "repair", "science"]
  id: root
  title: i18n.t("cmap.skills")
  width: 160 + 75 + 35

  signal pickedSkill(string skillName)

  content: ColumnLayout {
    spacing: 5

    ButtonNavigation {
      id: buttonNavigation
      buttonRepeater: repeater
      enabled: root.visible
      onEnabledChanged: currentIndex = -1
    }

    Repeater {
      id: repeater
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
          onClicked: {
            root.pickedSkill(root.skills[index])
            if (typeof soundManager != "undefined")
              soundManager.play("ui/tiny-btn-click");
          }
          onHoveredChanged: mouseCursor.setCurrentPointer(hovered ? MouseCursor.ActivePointer : MouseCursor.NormalPointer)
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
