import QtQuick 2.15

Repeater {
  property point origin

  model: gameManager.currentGame.level.textBubbles
  delegate: Rectangle {
    property QtObject textBubble: gameManager.currentGame.level.textBubbles[index]
    color: Qt.rgba(0, 0, 0, 0.5)
    border.color: "white"
    border.width: 1
    radius: 10
    width: 150
    height: textBubbleContentView.height
    opacity: textBubble.opacity

    x: textBubble.position.x + origin.x + (72 / 2) - width / 2
    y: textBubble.position.y + origin.y - height

    Text {
      id: textBubbleContentView
      padding: 5
      width: parent.width
      wrapMode: Text.WordWrap
      text: textBubble.content
      color: textBubble.color
    }
  }
}
