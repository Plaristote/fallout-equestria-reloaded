import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Game 1.0
import "../../assets/ui" as UiStyle
import "../../ui"

SideBar {
  property QtObject character
  property QtObject target
  property var spells: character.statistics.spells
  id: root
  title: i18n.t("spells.title")
  width: 160 + 75 + 35

  signal pickedSpell(string spellName)

  content: ColumnLayout {
    spacing: 5

    ButtonNavigation {
      id: buttonNavigation
      buttonRepeater: repeater
      enabled: root.visible
      onEnabledChanged: currentIndex = -1
    }

    UiStyle.TitleText {
      text: i18n.t("spells.empty-spellbook")
      color: "white"
      visible: spells.length === 0
      wrapMode: Text.WordWrap
      Layout.fillWidth: true
    }

    Repeater {
      model: root.spells
      delegate: Button {
        background: UiStyle.Label { style: parent.down ? "dark" : "base" }
        Layout.fillWidth: true
        text: i18n.t(`spells.${root.spells[index]}`)
        width: 160
        height: 40
        contentItem: UiStyle.TitleText {
          anchors.centerIn: parent
          text: parent.text
          color: "white"
          outline: "black"
        }
        onClicked: {
          root.pickedSpell(root.spells[index])
          if (typeof soundManager != "undefined")
            soundManager.play("ui/tiny-btn-click");
        }
        onHoveredChanged: mouseCursor.setCurrentPointer(hovered ? MouseCursor.ActivePointer : MouseCursor.NormalPointer)
      }
    }
  }
}
