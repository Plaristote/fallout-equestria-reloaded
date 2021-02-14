import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject characterSheet

  id: perksPane
  background: UiStyle.TerminalPane {}

  signal selectProperty(string selectedName)

  Column {
    Text { text: "Traits"; color: "lightgreen" }
    Rectangle { height: 1; width: perksPane.width; color: "lightgreen" }
    Repeater {
      model: characterSheet.traits
      delegate: Text {
        text: root.characterSheet.traits[index]; color: "lightgreen"
        font.family: application.consoleFontName
        font.pointSize: 8
        MouseArea { anchors.fill: parent; onClicked: selectProperty(characterSheet.traits[index]) }
      }
    }

    Rectangle { height: 5; width: perksPane.width; color: "transparent" }

    Text { text: "Perks"; color: "lightgreen" }
    Rectangle { height: 1; width: perksPane.width; color: "lightgreen" }
    Repeater {
      model: characterSheet.perks
      delegate: Text {
        text: root.characterSheet.traits[index]; color: "lightgreen"
        font.family: application.consoleFontName
        font.pointSize: 8
        MouseArea { anchors.fill: parent; onClicked: selectProperty(root.characterSheet.perks[index]) }
      }
    }
  }
}
