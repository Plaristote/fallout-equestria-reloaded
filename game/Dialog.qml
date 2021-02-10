import QtQuick 2.15
import QtQuick.Controls 2.15
import "qrc:/ui"
import "qrc:/assets/ui" as UiStyle

Item {
  id: root
  property QtObject character

  function exitDialog() {
    application.popView();
  }

  FaceDisplay {
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

  BorderImage {
    id: textDisplay
    source: "qrc:/assets/ui/dialog/text.png"
    border { top: 6; left: 287; right: 215; bottom: 11 }
    anchors { top: faceForeground.bottom; left: faceForeground.left; right: faceForeground.right }
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch
    height: 100

    Flickable {
      clip: true
      contentHeight: dialogContent.height
      anchors.fill: parent
      anchors.leftMargin: 290
      anchors.rightMargin: 220
      anchors.topMargin: 8
      anchors.bottomMargin: 13

      ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

      Text {
        id: dialogContent
        color: "white"
        text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. In risus tellus, commodo vitae pretium at, viverra elementum velit. Phasellus et finibus magna. Praesent fermentum, tortor eu vehicula hendrerit, magna sem iaculis metus, vitae placerat quam ligula vitae nibh. In accumsan sodales tortor, nec efficitur tortor bibendum in. In hac habitasse platea dictumst. Vivamus pellentesque ex sit amet mauris fringilla porta. Phasellus mollis enim nec lacus tempus, sit amet pretium nulla vestibulum. Suspendisse non tortor quis tortor auctor posuere nec a ligula. Suspendisse a lectus sollicitudin, feugiat dolor consectetur, tempus ligula. Etiam at diam at quam rutrum fringilla. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam facilisis sodales eros eget viverra."
        wrapMode: Text.WordWrap
        horizontalAlignment: Qt.AlignJustify
        width: parent.width - 15
      }
    }
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

  BorderImage {
    id: answersPane
    source: "qrc:/assets/ui/dialog/center.png"
    border { top: 41; left: 80; right: 58; bottom: 31 }
    anchors { left: leftPane.right; top: textDisplay.bottom; right: rightPane.left; bottom: parent.bottom }
    horizontalTileMode: BorderImage.Stretch
    verticalTileMode: BorderImage.Stretch

    Flickable {
      id: answersFlickable
      clip: true
      contentHeight: answersList.height
      anchors.fill: parent
      anchors.leftMargin: 80
      anchors.rightMargin: 68
      anchors.topMargin: 51
      anchors.bottomMargin: 41

      ScrollBar.vertical: UiStyle.TerminalScrollbar { orientation: Qt.Vertical }

      Column {
        id: answersList
        Repeater {
          model: 100
          delegate: Button {
            text: `> Words ^ ${index}`
            hoverEnabled: true
            contentItem: Text { color: parent.hovered ? "white" : "green"; text: parent.text }
            background: Rectangle { color: "transparent" }
            onClicked: root.exitDialog()
            width: answersFlickable.width - 10
          }
        }
      }
    }
  }
}
