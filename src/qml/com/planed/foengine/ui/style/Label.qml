import QtQuick 2.15

Item {
  readonly property string imageSource: "qrc:/assets/ui/hud.png"
  readonly property real imageX: 204
  readonly property real imageY: 32
  readonly property real imageH: 26
  readonly property real imageW: 168
  readonly property real borderSize: 8

  readonly property rect baseCenter: Qt.rect(imageX + borderSize, imageY, imageW - borderSize * 2, imageH)
  readonly property rect baseLeft:   Qt.rect(imageX, imageY, borderSize, imageH)
  readonly property rect baseRight:  Qt.rect(imageX + imageW - borderSize, imageY, borderSize, imageH)

  readonly property rect lightCenter: Qt.rect(baseCenter.left, baseCenter.top + (imageH + 2), baseCenter.width, baseCenter.height)
  readonly property rect lightLeft:   Qt.rect(baseLeft.left,   baseLeft.top   + (imageH + 2), baseLeft.width,   baseLeft.height)
  readonly property rect lightRight:  Qt.rect(baseRight.left,  baseRight.top  + (imageH + 2), baseRight.width,  baseRight.height)

  readonly property rect darkCenter: Qt.rect(baseCenter.left, baseCenter.top + (imageH + 2) * 2, baseCenter.width, baseCenter.height)
  readonly property rect darkLeft:   Qt.rect(baseLeft.left,   baseLeft.top   + (imageH + 2) * 2, baseLeft.width,   baseLeft.height)
  readonly property rect darkRight:  Qt.rect(baseRight.left,  baseRight.top  + (imageH + 2) * 2, baseRight.width,  baseRight.height)

  readonly property var styleDesc: ({
    "base":  { "center": baseCenter, "left": baseLeft, "right": baseRight },
    "light": { "center": lightCenter, "left": lightLeft, "right": lightRight },
    "dark":  { "center": darkCenter, "left": darkLeft, "right": darkRight }
  })

  property string style: "base"

  Image {
    source: imageSource
    sourceClipRect: styleDesc[style].center
    anchors {
      top: parent.top
      bottom: parent.bottom
      left: borderLeft.right
      right: borderRight.left
    }
  }

  Image {
    id: borderLeft
    sourceClipRect: styleDesc[style].left
    source: imageSource
    anchors {
      top: parent.top
      bottom: parent.bottom
      left: parent.left
    }
  }

  Image {
    id: borderRight
    sourceClipRect: styleDesc[style].right
    source: imageSource
    anchors {
      top: parent.top
      bottom: parent.bottom
      right: parent.right
    }
  }
}
