import QtQuick 2.15

Image {
  property QtObject model
  visible:        model !== null
  source:         model !== null ? fileProtocol + model.getSpriteSource() : ""
  sourceClipRect: model !== null ? model.getClippedRect() : Qt.rect(0,0,0,0)
  height:         sourceClipRect.height
  width:          sourceClipRect.width
  fillMode:       Image.PreserveAspectCrop
}
