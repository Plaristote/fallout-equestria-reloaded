import QtQuick 2.15
import QtGraphicalEffects 1.15

Image {
  id: root
  readonly property string basePath: "qrc:/assets/faces"
  property string theme:    "mare-basic"
  property string mood:     "neutral"
  property string ambiance: "wasteland"
  property color  color:    Qt.rgba(0, 205, 80, 0.5)
  property var    accessories: ["fancypants","eye-scar"]

  source: basePath + "/backgrounds/" + ambiance + ".jpg"
  fillMode: Image.PreserveAspectCrop

  Image {
    id: face
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    source: basePath + '/' + theme + "/base.png"
    fillMode: Image.PreserveAspectFit

    height: Math.min(parent.height - 20, sourceSize.height)

    ColorOverlay {
      anchors.fill: parent
      source: parent
      color: root.color
    }

    Image {
      anchors.fill: parent
      source: basePath + '/' + theme + '/eyes/' + mood + '.png'
      fillMode: Image.Stretch
    }

    Image {
      anchors.fill: parent
      source: basePath + '/' + theme + '/mouthes/' + mood + '.png'
      fillMode: Image.Stretch

      ColorOverlay {
        anchors.fill: parent
        source: parent
        color: root.color
      }
    }

    Repeater {
      model: root.accessories
      delegate: Image {
        anchors.fill: face
        source: basePath + '/' + theme + '/accessories/' + root.accessories[index] + '.png'
        fillMode: Image.Stretch
      }
    }
  }
}
