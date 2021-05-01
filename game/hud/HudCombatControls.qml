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
      PropertyChanges { target: overlay; y: root.y - root.height }
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

  onStateChanged: console.log("ON ZTATE CHANGED MAGGLE", state)

  Column {
    anchors.centerIn: parent

    UiStyle.TinyButton {
      text: i18n.t("Pass turn")
      onClicked: levelController.passTurn(levelController.player)
    }
  }

 Image {
   id: overlay
   source: "qrc:/assets/ui/hud.png"
   sourceClipRect: Qt.rect(130, 204, 75, 32)
   width: parent.width
   height: parent.height
 }
}
