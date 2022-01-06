import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/assets/ui" as UiStyle

Pane {
  id: root
  clip: true
  background: Image { source: "qrc:/assets/ui/pane.png" }

  state: "no-combat"

  states: [
    State {
      name: "in-combat"; when: levelController.combat
      PropertyChanges { target: overlay; y: root.y - root.height - 20 }
    },
    State { name: "no-combat"; when: !levelController.combat }
  ]

  transitions: [
    Transition {
      to: "in-combat"
      PropertyAnimation { target: overlay; property: "y" }
    },
    Transition {
      to: "no-combat"
      PropertyAnimation { target: overlay; property: "y" }
    }
  ]

  Column {
    anchors.centerIn: parent
    spacing: 5

    Row {
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: 5
      UiStyle.TitleText {
        text: i18n.t("AP")
        font.pixelSize: 16
        color: "white"
        outline: "black"
        anchors.verticalCenter: parent.verticalCenter
      }
      UiStyle.TerminalPane {
        Text {
          text: `${gameManager.currentGame.player.actionPoints}/${gameManager.currentGame.player.statistics.actionPoints}`
          font.family: application.titleFontName
          font.pixelSize: 16
          color: "white"
          anchors.centerIn: parent
        }
        height: 28
        width:  55
      }
    }

    UiStyle.TinyButton {
      text: i18n.t("Pass turn")
      onClicked: levelController.passTurn(levelController.player)
    }
  }

 Image {
   id: overlay
   source: "qrc:/assets/ui/hud.png"
   sourceClipRect: Qt.rect(130, 204, 75, 32)
   width: parent.width + 20
   height: parent.height + 20
   x: -10
   y: -10
 }
}
