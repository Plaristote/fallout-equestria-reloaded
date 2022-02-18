import QtQuick 2.15
import Game 1.0

Rectangle {
  property int previewFrame: 0
  property string spriteGroup: ""
  property string animationName: ""
  property alias nativeHeight: spriteAnimation.clippedRect.height
  property alias nativeWidth:  spriteAnimation.clippedRect.width
  id: root
  implicitWidth:  spriteAnimation.clippedRect.height
  implicitHeight: spriteAnimation.clippedRect.width
  border.color: "green"
  border.width: 2
  color: "transparent"

  onSpriteGroupChanged:   spriteAnimation.initialize(spriteGroup, animationName)
  onAnimationNameChanged: spriteAnimation.initialize(spriteGroup, animationName)
  Component.onCompleted:  spriteAnimation.initialize(spriteGroup, animationName)

  SpriteAnimation {
    id: spriteAnimation
    frameCount: 0
    frameInterval: 1000
  }

  Image {
    source: assetPath + "sprites/" + spriteAnimation.relativeSource
    anchors.fill: parent
    sourceClipRect: spriteAnimation.rectForFrame(root.previewFrame)
    Timer {
      interval: spriteAnimation.frameInterval
      running: animationName != ""
      repeat: true
      onTriggered: {
        root.previewFrame = root.previewFrame + 1 >= spriteAnimation.frameCount ? 0 : root.previewFrame + 1;
      }
    }
  }
}
