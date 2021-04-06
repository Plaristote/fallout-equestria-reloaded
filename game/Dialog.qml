import QtQuick 2.15
import QtQuick.Controls 2.15
import "../ui"
import "../ui/dialog"
import "qrc:/assets/ui" as UiStyle

Item {
  id: root
  property QtObject controller

  Connections {
    target: controller
    function onDialogEnded() {
      application.popView();
      gameManager.currentGame.level.paused = false;
    }
  }

  FaceDisplay {
    mood:        controller.mood
    ambiance:    controller.ambiance
    theme:       controller.npc.statistics.faceTheme
    accessories: controller.npc.statistics.faceAccessories
    color:       controller.npc.statistics.faceColor
    anchors.fill: faceForeground
    anchors.topMargin: 12
    anchors.leftMargin: 287
    anchors.rightMargin: 220
    anchors.bottomMargin: 6
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

  Image {
    id: leftPane
    source: "qrc:/assets/ui/dialog/left.png"
    anchors { top: textDisplay.bottom; bottom: parent.bottom; left: parent.left; }
    width: 135
    fillMode: Image.Stretch
  }

  Image {
    id: rightPane
    source: "qrc:/assets/ui/dialog/right.png"
    anchors { top: textDisplay.bottom; bottom: parent.bottom; right: parent.right; }
    width: 132
    fillMode: Image.Stretch
  }

  DialogAnswersDisplay {
    id: answersPane
    anchors { left: leftPane.right; top: textDisplay.bottom; right: rightPane.left; bottom: parent.bottom }
    sourceComponent: Column {
      id: answersList
      width: parent.width
      Repeater {
        model: root.controller.options
        delegate: Button {
          text: "> " + root.controller.getOptionText(controller.options[index])
          font.family: application.consoleFontName
          hoverEnabled: true
          contentItem: Text { color: parent.hovered ? "white" : "green"; text: parent.text; font: parent.font; wrapMode: Text.WordWrap }
          background: Rectangle { color: "transparent" }
          onClicked: root.controller.selectOption(controller.options[index])
          width: answersList.width - 10
        }
      }
    }
  }
}