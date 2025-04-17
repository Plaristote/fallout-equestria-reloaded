import QtQuick 2.15
import QtQuick.Controls 2.15
import "../ui"
import "../ui/dialog"

Item {
  id: root
  property QtObject controller
  property int bottomPartY: textDisplay.y + textDisplay.height
  property int bottomPartHeight: parent.height - bottomPartY

  Connections {
    target: controller
    function onDialogEnded() {
      if (root.StackView.status === StackView.Active)
        application.popView();
      if (gameManager.currentGame.level)
        gameManager.currentGame.level.paused = false;
    }
    function onBarterStarted() { root.state = "barter"; }
    function onBarterEnded()   { root.state = "dialog"; }
  }

  states: [
    State {
      name: "dialog"
      PropertyChanges { target: barterMode; y: root.height }
      PropertyChanges { target: dialogMode; y: bottomPartY }
    },
    State {
      name: "barter"
      PropertyChanges { target: dialogMode; y: root.height }
      PropertyChanges { target: barterMode; y: bottomPartY }
    }
  ]

  state: "dialog"

  transitions: [
    Transition {
      from: "dialog"
      to: "barter"
      SequentialAnimation {
        PropertyAnimation { target: dialogMode; property: "y"; from: bottomPartY; to: root.height }
        PropertyAnimation { target: barterMode; property: "y"; from: root.height; to: bottomPartY }
      }
    },
    Transition {
      from: "barter"
      to: "dialog"
      SequentialAnimation {
        PropertyAnimation { target: barterMode; property: "y"; from: bottomPartY; to: root.height }
        PropertyAnimation { target: dialogMode; property: "y"; from: root.height; to: bottomPartY }
      }
    }
  ]

  Loader {
    anchors.fill: faceForeground
    anchors.topMargin: 12
    anchors.leftMargin: 287
    anchors.rightMargin: 220
    anchors.bottomMargin: 6
    sourceComponent: controller.npc.getObjectType() == "Character" ? faceDisplay : objectDisplay
  }

  Component {
    id: faceDisplay
    FaceDisplay {
      mood:        controller.mood
      ambiance:    controller.ambiance
      theme:       controller.npc.statistics.faceTheme
      hairStyle:   controller.npc.statistics.hairTheme
      accessories: controller.npc.statistics.faceAccessories
      color:       controller.npc.statistics.faceColor
      coloured:    controller.npc.statistics.withFaceColor
      hairColor:   controller.npc.statistics.hairColor
    }
  }

  Component {
    id: objectDisplay
    Item {
      Image {
        anchors.centerIn: parent
        source: controller.npc.spriteSource.length > 0 ? "file://" + controller.npc.spriteSource : ""
        height: controller.npc.clippedRect.height
        width:  controller.npc.clippedRect.width
        sourceClipRect: controller.npc.clippedRect
      }
    }
  }

  BorderImage {
    id: faceForeground
    source: "qrc:/assets/ui/dialog/top.png"
    border { top: 12; left: 287; right: 220; bottom: 6 }
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch
    width:  parent.width
    height: parent.height / 2
  }

  DialogTextDisplay {
    id: textDisplay
    anchors { top: faceForeground.bottom; left: faceForeground.left; right: faceForeground.right }
    text: root.controller.text
  }

  DialogAnswersPane {
    id: dialogMode
    controller: root.controller
    enabled: root.state == "dialog"
  }

  Loader {
    id: barterMode
    anchors { left: parent.left; right: parent.right }
    y: parent.height
    height: bottomPartHeight
    sourceComponent: controller.npc.getObjectType() === "Character" ? barterComponent : null
  }

  Component {
    id: barterComponent
    Barter {
      controller: root.controller.barter
      onClosed: root.controller.barterEnded()
    }
  }

  FastTimePassView {
    delay: 0
  }
}
