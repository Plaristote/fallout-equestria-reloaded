import QtQuick 2.15
import QtQuick.Controls 2.15
import "../ui"
import "../ui/dialog"
import "qrc:/assets/ui" as UiStyle

Item {
  property QtObject controller
  property alias currentIndex: buttonNavigation.currentIndex
  id: root
  y: bottomPartY
  height: bottomPartHeight
  anchors { left: parent.left; right: parent.right }

  UiStyle.ButtonNavigation {
    id: buttonNavigation
    enabled: root.enabled
  }

  function select(index) {
    controller.selectOption(controller.options[index])
    if (typeof soundManager != "undefined")
      soundManager.play("ui/term-btn-click");
  }

  Connections {
    target: gamepad
    enabled: root.enabled
    function onInventoryClicked() { controller.tryToBarter() }
  }

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
      Component.onCompleted: {
        buttonNavigation.buttonRepeater = repeater
      }
      Repeater {
        id: repeater
        model: root.controller.options
        delegate: Button {
          text: "> " + root.controller.getOptionText(controller.options[index])
          font.family: application.consoleFontName
          font.pointSize: application.consoleFont.bigSize
          hoverEnabled: true
          contentItem: Text {
            color: root.currentIndex === index ? "white" : "green"
            text: parent.text
            font: parent.font
            wrapMode: Text.WordWrap
          }
          background: Rectangle { color: "transparent" }
          onClicked: root.select(index);
          onHoveredChanged: if (hovered) { root.currentIndex = index; }
          width: answersList.width - 10
        }
      }
    }
  }
}
