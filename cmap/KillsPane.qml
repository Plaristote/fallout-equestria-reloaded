import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject characterSheet
  property var killedRaces: characterSheet.getKilledRaces()
  property string selectedProperty

  id: killsPane
  background: UiStyle.TerminalPane {}

  Column {
    width: parent.width
    spacing: 2
    Repeater {
      model: characterSheet.getKilledRaces()
      delegate: Row {
        Text {
          width: killsPane.width - countText.width - 25
          text: killedRaces[index]
          font.family: application.consoleFont.name
          font.pointSize: application.consoleFont.tinySize
          color: "white"
        }
        Text {
          id: countText
          text: characterSheet.getKillCount(killedRaces[index])
          font.family: application.consoleFont.name
          font.pointSize: application.consoleFont.tinySize
          color: "white"
        }
      }
    }
  }
}
