import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/assets/ui" as UiStyle

Pane {
  property QtObject dataEngine: gameManager.currentGame.dataEngine
  property var factions: dataEngine.getFactionList()
  property string selectedProperty

  signal selectProperty(string selectedName)

  function reputationCategory(value) {
    if (value >= 25 && value < 100)
      return "liked";
    if (value >= 100 && value < 150)
      return "liked+";
    if (value >= 150 && value < 200)
      return "liked++";
    if (value >= 200)
      return "max";
    if (value <= -25 && value > -100)
      return "disliked"
    if (value <= -100 && value > -150)
      return "disliked+";
    if (value <= -150 && value > -200)
      return "disliked++";
    if (value <= -200)
      return "min";
    return "neutral";
  }

  background: UiStyle.TerminalPane {}

  GridLayout {
    columns: 2
    rowSpacing: 3
    width: parent.width

    Repeater {
      model: factions.length * 2
      delegate: Text {
        property string faction: factions[Math.floor(index / 2)]
        property int label: !(index % 2)
        property int reputation: dataEngine.getReputation(faction)
        property string category: reputationCategory(reputation)
        property string propertyName: `factions.${faction}`

        text: label ? i18n.t(`factions.${faction}.name`) : i18n.t(`reputation.${category}`)
        visible: dataEngine.hasReputation(faction)
        font.family: application.consoleFont.name
        font.pointSize: application.consoleFont.tinySize
        color: selectedProperty === propertyName ? "green" : "white"
        Layout.fillWidth: label
        MouseArea {
          anchors.fill: parent
          onClicked: selectProperty(propertyName)
        }
      }
    }
  }
}
