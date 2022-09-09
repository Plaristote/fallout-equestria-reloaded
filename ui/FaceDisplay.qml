import QtQuick 2.15
import QtGraphicalEffects 1.15

Image {
  id: root
  readonly property string basePath: `${assetPath}faces`
  property string theme:     "mare-basic"
  property string hairStyle: ""
  property string mood:      "neutral"
  property string ambiance:  "wasteland"
  property bool   coloured:  false
  property color  color:     Qt.rgba(0, 205, 80, 0.5)
  property color  eyeColor:  Qt.rgba(255, 0, 0)
  property color  hairColor: Qt.rgba(255, 255, 0)
  property var    accessories: ["fancypants","eye-scar"]
  property real   breathingSpeed: 2048
  property string moodSource: mood == "sleep" ? "neutral" : mood

  source: basePath + "/backgrounds/" + ambiance + ".png"
  fillMode: Image.PreserveAspectCrop
  clip: true

  Component.onCompleted: {
    scheduleNextBlink();
    breathingInhale.running = true;
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

    PropertyAnimation on anchors.bottomMargin {
      id: breathingInhale
      from: 0
      to: mood == "sleep" ? 0 : -8
      duration: breathingSpeed
      onFinished: breathingExhale.running = true
    }

    PropertyAnimation on anchors.bottomMargin {
      id: breathingExhale
      from: mood == "sleep" ? 0 : -8
      to: 0
      duration: breathingSpeed * 1.42
      onFinished: breathingInhale.running = true
    }

    ColorOverlay {
      visible: coloured
      anchors.fill: parent
      source: parent
      color: root.color
    }

    Image {
      id: faceOverlay
      anchors.fill: parent
      source: basePath + '/' + theme + '/base-overlay.png'
      fillMode: Image.PreserveAspectFit
    }

    Image {
      id: eyes
      anchors.fill: parent
      source: basePath + '/' + theme + '/eyes/' + moodSource + '.png'
      fillMode: Image.Stretch
    }

    Image {
      id: eyesColor
      anchors.fill: parent
      source: basePath + '/' + theme + '/eye-colors/' + moodSource + '.png'

      ColorOverlay {
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
        source: basePath + '/' + theme + '/eyes/' + moodSource + '.png'
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: eyes.height

        ColorOverlay {
          anchors.fill: parent
          source: parent
          color: Qt.rgba(root.color.r, root.color.g, root.color.b, 1)
        }
      }

      PropertyAnimation on height {
        id: openEyelids
        from: eyes.height
        to: mood != "sleep" ? 0 : eyes.height
        duration: 200
        onFinished: scheduleNextBlink()
      }

      PropertyAnimation on height {
        id: closeEyelids
        from: mood != "sleep" ? 0 : eyes.height
        to: eyes.height
        duration: 200
        onFinished: openEyelids.running = true
      }
    }

    Image {
      anchors.fill: parent
      source: basePath + '/' + theme + '/eyelids/' + moodSource +  '.png'
      fillMode: Image.Stretch
    }

    Image {
      anchors.fill: parent
      source: basePath + '/' + theme + '/mouthes/' + moodSource + '.png'
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

    Loader {
      anchors.fill: parent
      sourceComponent: hairStyle != "" ? hairStyleDisplay : null
    }

    Component {
      id: hairStyleDisplay
      Image {
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
}
