import QtQuick 2.15
import QtQuick.Controls 2.15
import "../ui"
import "../ui/dialog"
import "qrc:/assets/ui" as UiStyle

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

  Item {
    id: dialogMode
    y: bottomPartY
    height: bottomPartHeight
    anchors { left: parent.left; right: parent.right }

    Image {
      id: leftPane
      source: "qrc:/assets/ui/dialog/left.png"
      anchors { top: parent.top; bottom: parent.bottom; left: parent.left; }
      width: 135
      fillMode: Image.Stretch
    }

    Image {
      id: rightPane
      source: "qrc:/assets/ui/dialog/right.png"
      anchors { top: parent.top; bottom: parent.bottom; right: parent.right; }
      width: 132
      fillMode: Image.Stretch

      UiStyle.PushButton {
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        text: i18n.t("Barter")
        onClicked: controller.tryToBarter()
      }
    }

    DialogAnswersDisplay {
      id: answersPane
      anchors { left: leftPane.right; top: parent.top; right: rightPane.left; bottom: parent.bottom }
      sourceComponent: Column {
        id: answersList
        width: parent.width
        Repeater {
          model: root.controller.options
          delegate: Button {
            text: "> " + root.controller.getOptionText(controller.options[index])
            font.family: application.consoleFontName
            font.pointSize: application.consoleFont.bigSize
            hoverEnabled: true
            contentItem: Text { color: parent.hovered ? "white" : "green"; text: parent.text; font: parent.font; wrapMode: Text.WordWrap }
            background: Rectangle { color: "transparent" }
            onClicked: {
              root.controller.selectOption(controller.options[index])
              if (typeof soundManager != "undefined") soundManager.play("ui/term-btn-click");
            }
            width: answersList.width - 10
          }
        }
      }
    }
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
