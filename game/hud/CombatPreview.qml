import QtQuick 2.15
import "../../ui"

Row {
  property QtObject levelController

  spacing: 10
  Repeater {
    model: levelController.combattants
    delegate: Rectangle {
      property QtObject character: levelController.combattants[index]
      property bool isSelf:  character === levelController.player
      property bool isAlly:  character.isAlly(levelController.player)
      property bool isEnemy: character.isEnemy(levelController.player)

      height: 20
      width: 20
      border.width: 2
      border.color: levelController.turn === index ? "yellow" : "black"
      color: {
        if (isSelf)
          return "lightgreen";
        else if (isAlly)
          return "green";
        else if (isEnemy)
          return "red";
        return "white";
      }

      CustomLabel {
        anchors.centerIn: parent
        visible: character.unconscious
        text: "KO"
      }

      MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: levelController.cameraFocusRequired(character)
      }

      Column {
        visible: mouseArea.containsMouse
        anchors {
          bottom: parent.top
          bottomMargin: 10
          horizontalCenter: parent.horizontalCenter
        }
        CustomLabel {
          text: character.statistics.name
        }
        CustomLabel {
          text:  `Sequence: ${character.statistics.sequence}`
        }
      }
    }
  }
}
