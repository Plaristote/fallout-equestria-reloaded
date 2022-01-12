import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  id: root
  property QtObject character
  property QtObject target
  property var spells: character.statistics.spells

  signal pickedSpell(string spellName)

  background: UiStyle.Pane {}
  width: 160 + 75 + 35

  ColumnLayout {
    spacing: 5

    UiStyle.TitleText {
      text: i18n.t("spells.title")
      color: "white"
      font.pointSize: 20
    }

    UiStyle.TitleText {
      text: i18n.t("spells.empty-spellbook")
      color: "white"
      visible: spells.length === 0
      font.pointSize: 20
    }

    Repeater {
      model: root.spells
      delegate: Button {
        background: UiStyle.Label { style: parent.down ? "dark" : "base" }
        text: i18n.t(`spells.${root.spells[index]}`)
        width: 160
        height: 40
        contentItem: UiStyle.TitleText {
          anchors.centerIn: parent
          text: parent.text
          color: "white"
          outline: "black"
          font.pointSize: 18
        }
        onClicked: root.pickedSpell(root.spells[index])
      }
    }
  }
}
