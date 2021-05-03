import QtQuick 2.15
import QtGraphicalEffects 1.15

Image {
  id: root
  readonly property string basePath: `${assetPath}/faces`
  property string theme:     "mare-basic"
  property string hairStyle: ""
  property string mood:      "neutral"
  property string ambiance:  "wasteland"
  property bool   coloured:  false
  property color  color:     Qt.rgba(0, 205, 80, 0.5)
  property color  eyeColor:  Qt.rgba(255, 0, 0)
  property color  hairColor: Qt.rgba(255, 255, 0)
  property var    accessories: ["fancypants","eye-scar"]

  source: basePath + "/backgrounds/" + ambiance + ".png"
  fillMode: Image.PreserveAspectCrop

  Component.onCompleted: {
    scheduleNextBlink();
  }

  function scheduleNextBlink() {
    if (Math.ceil(Math.random() * 15) > 2)
    {
      const seconds = Math.ceil(Math.random() * 8);

      blinkTimer.interval = seconds * 1000;
      blinkTimer.running = true;
    }
    else
      closeEyelids.running = true;
  }

  Timer {
    id: blinkTimer
    onTriggered: closeEyelids.running = true
  }

  Image {
    id: face
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    source: basePath + '/' + theme + "/base.png"
    fillMode: Image.PreserveAspectFit

    height: Math.min(parent.height - 20, sourceSize.height)

    ColorOverlay {
      visible: coloured
      anchors.fill: parent
      source: parent
      color: root.color
    }

    Image {
      id: eyes
      anchors.fill: parent
      source: basePath + '/' + theme + '/eyes/' + mood + '.png'
      fillMode: Image.Stretch
    }

    Image {
      id: eyesColor
      anchors.fill: parent
      source: basePath + '/' + theme + '/eye-colors/' + mood + '.png'

      ColorOverlay {
        visible: coloured
        anchors.fill: parent
        source: parent
        color: Qt.rgba(eyeColor.r, eyeColor.g, eyeColor.b, 0.5)
      }
    }

    Item {
      anchors { top: eyes.top; left: eyes.left; right: eyes.right }
      clip: true

      Image {
        id: eyelids
        source: basePath + '/' + theme + '/eyes/' + mood + '.png'
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: eyes.height

        ColorOverlay {
          visible: coloured
          anchors.fill: parent
          source: parent
          color: Qt.rgba(root.color.r, root.color.g, root.color.b, 1)
        }
      }

      PropertyAnimation on height {
        id: openEyelids
        from: eyes.height
        to: 0
        duration: 200
        onFinished: scheduleNextBlink()
      }

      PropertyAnimation on height {
        id: closeEyelids
        from: 0
        to: eyes.height
        duration: 200
        onFinished: openEyelids.running = true
      }
    }

    Image {
      anchors.fill: parent
      source: basePath + '/' + theme + '/eyelids/' + mood +  '.png'
      fillMode: Image.Stretch
    }

    Image {
      anchors.fill: parent
      source: basePath + '/' + theme + '/mouthes/' + mood + '.png'
      fillMode: Image.Stretch

      ColorOverlay {
        visible: coloured
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

    Image {
      anchors.fill: parent
      source: basePath + '/' + theme + '/hairstyles/' + hairStyle + '.png'
      fillMode: Image.Stretch

      ColorOverlay {
        visible: coloured
        anchors.fill: parent
        source: parent
        color: Qt.rgba(hairColor.r, hairColor.g, hairColor.b, 0.5)
      }
    }
  }
}
