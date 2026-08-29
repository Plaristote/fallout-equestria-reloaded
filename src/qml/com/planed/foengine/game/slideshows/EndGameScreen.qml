import QtQuick 2.15

Rectangle {
  id: root
  color: "black"

  property var callback: null
  property var slides: []
  property int currentSlideIndex: -1
  property bool isPlaying: true
  readonly property var currentSlide: root.slides[root.currentSlideIndex]

  readonly property int slideDuration: 5000  // ms per slide
  readonly property int fadeInDuration: 1000
  readonly property int fadeOutDuration: 1000
  readonly property color subtitleColor: "#f0e6d2"
  readonly property color vignetteColor: "#1a0f0a"

  // Vignette overlay
  Rectangle {
    anchors.fill: parent
    gradient: Gradient {
      GradientStop { position: 0.0; color: "transparent" }
      GradientStop { position: 0.7; color: "transparent" }
      GradientStop { position: 1.0; color: Qt.rgba(0.1, 0.06, 0.04, 0.6) }
    }
  }

  // Slide image 
  Image {
    id: slideImage
    anchors.fill: parent
    fillMode: Image.PreserveAspectFit
    opacity: 0
    smooth: true

    Rectangle {
      anchors.fill: parent
      color: "#3d2817"
      opacity: 0.15
    }
  }

  // CRT effect
  Item {
    anchors.fill: parent
    opacity: 0.05

    Repeater {
      model: Math.floor(parent.height / 4)
      Rectangle {
        width: parent.width
        height: 2
        y: index * 4
        color: "#000000"
      }
    }
  }

  Rectangle {
    id: subtitleContainer
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottomMargin: 60
    width: parent.width - 120
    height: subtitleText.contentHeight + 40
    color: Qt.rgba(0, 0, 0, 0.85)
    border.color: "#4a3829"
    border.width: 2
    opacity: 0

    Text {
      id: subtitleText
      anchors.centerIn: parent
      width: parent.width - 40
      horizontalAlignment: Text.AlignHCenter
      wrapMode: Text.WordWrap
      font.family: "Courier New"
      font.pixelSize: 24
      font.weight: Font.Bold
      color: root.subtitleColor
      textFormat: Text.PlainText
      style: Text.Outline
      styleColor: "#1a1410"
    }
  }

  NumberAnimation {
    id: fadeInAnimation
    target: slideImage
    property: "opacity"
    from: 0
    to: 1
    duration: root.fadeInDuration
    easing.type: Easing.InOutQuad
  }

  NumberAnimation {
    id: fadeOutAnimation
    target: slideImage
    property: "opacity"
    from: 1
    to: 0
    duration: root.fadeOutDuration
    easing.type: Easing.InOutQuad
    onFinished: {
      if (root.isPlaying) {
        root.nextSlide()
      }
    }
  }

  NumberAnimation {
    id: subtitleFadeIn
    target: subtitleContainer
    property: "opacity"
    from: 0
    to: 1
    duration: 800
    easing.type: Easing.InOutQuad
  }

  NumberAnimation {
    id: subtitleFadeOut
    target: subtitleContainer
    property: "opacity"
    from: 1
    to: 0
    duration: 600
    easing.type: Easing.InOutQuad
  }

  Timer {
    id: slideTimer
    interval: root.slideDuration
    onTriggered: {
      if (root.isPlaying) {
        subtitleFadeOut.start();
        fadeOutAnimation.start();
      }
    }
  }

  Timer {
    id: subtitleDelayTimer
    interval: 400
    onTriggered: {
      if (root.isPlaying && root.currentSlide) {
        if (root.currentSlide.subtitle) {
          subtitleText.text = root.currentSlide.subtitle;
          subtitleFadeIn.start();
        }
      }
    }
  }

  onSlidesChanged: {
    root.isPlaying = false;
    root.currentSlideIndex = -1;
    root.isPlaying = true;
    root.nextSlide();
  }

  function pause() {
    root.isPlaying = false;
    slideTimer.stop();
    fadeInAnimation.stop();
    fadeOutAnimation.stop();
    subtitleFadeIn.stop();
    subtitleFadeOut.stop();
  }

  function stop() {
    root.isPlaying = false;
    slideTimer.stop();
    fadeInAnimation.stop();
    fadeOutAnimation.stop();
    subtitleFadeIn.stop();
    subtitleFadeOut.stop();
    slideImage.opacity = 0;
    subtitleContainer.opacity = 0;
    root.currentSlideIndex = -1;
  }

  function nextSlide() {
    root.currentSlideIndex++;
    if (root.currentSlideIndex >= root.slides.length) {
      root.stop();
      callback();
      return ;
    }
    slideImage.source = root.currentSlide.image;
    subtitleContainer.opacity = 0;
    subtitleText.text = "";
    fadeInAnimation.start();
    if (root.currentSlide.subtitle)
      subtitleDelayTimer.start();
    slideTimer.interval = root.currentSlide.duration || root.currentSlideDuration;
    slideTimer.start()
  }

  function previousSlide() {
    if (root.currentSlideIndex > 0) {
      [slideTimer, fadeOutAnimation, subtitleFadeOut].forEach(object => object.stop());
      root.currentSlideIndex -= 2;
      slideImage.opacity = 0;
      subtitleContainer.opacity = 0;
      root.nextSlide();
    }
  }

  Keys.onPressed: {
    event.accepted = true;
    if (event.key === Qt.Key_Space) {
      if (root.isPlaying) {
        root.pause();
      } else {
        root.play();
      }
    } else if (event.key === Qt.Key_Right) {
      subtitleFadeOut.stop();
      subtitleContainer.opacity = 0;
      fadeOutAnimation.stop();
      slideImage.opacity = 0;
      slideTimer.stop();
      root.nextSlide();
    } else if (event.key === Qt.Key_Left) {
      root.previousSlide();
    } else if (event.key === Qt.Key_Escape) {
      root.stop();
    } else {
      event.accepted = false;
    }
  }
}
