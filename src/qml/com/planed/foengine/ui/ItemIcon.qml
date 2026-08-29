import QtQuick 2.15

Image {
  property QtObject model
  visible:        model !== null
  source:         model !== null ? `${assetPath}icons/${model.icon}` : ""
  fillMode:       Image.PreserveAspectCrop
}
