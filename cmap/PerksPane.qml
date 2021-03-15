import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject characterSheet

  id: perksPane
  background: UiStyle.TerminalPane {}

  signal selectProperty(string selectedName)

  Column {
    Text { text: i18n.t("cmap.traits"); color: "lightgreen" }
    Rectangle { height: 1; width: perksPane.width - 25; color: "lightgreen" }
    Repeater {
      model: characterSheet.traits
      delegate: Text {
        text: i18n.t("cmap." + root.characterSheet.traits[index]); color: "lightgreen"
        font.family: application.consoleFontName
        font.pointSize: 8
        MouseArea { anchors.fill: parent; onClicked: selectProperty(characterSheet.traits[index]) }
      }
    }

    Rectangle { height: 5; width: perksPane.width; color: "transparent" }

    Text { text: i18n.t("cmap.perks"); color: "lightgreen" }
    Rectangle { height: 1; width: perksPane.width - 25; color: "lightgreen" }
    Repeater {
      model: characterSheet.perks
      delegate: Text {
        text: i18n.t("cmap." + root.characterSheet.traits[index]); color: "lightgreen"
        font.family: application.consoleFontName
        font.pointSize: 8
        MouseArea { anchors.fill: parent; onClicked: selectProperty(root.characterSheet.perks[index]) }
      }
    }
  }
}
