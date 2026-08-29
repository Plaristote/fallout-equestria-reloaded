import QtQuick 2.15
import QtMultimedia 5.15

Item {
  id: root
  property QtObject gameController
  property QtObject levelController
  property string source
  property int elapsingTime
  property bool pausedLevel: false
  anchors.fill: parent

  Component.onCompleted: {
    if (levelController)
      pausedLevel = !levelController.paused;
    if (pausedLevel)
      levelController.paused = true;
    gameController.worldmap.paused = true;
    musicManager.pause(true);
  }

  function onDone() {
    console.log("Video stopped, going back to level");
    gameController.advanceTime(elapsingTime);
    application.popView();
    if (pausedLevel)
      levelController.paused = false;
    gameController.worldmap.paused =  false;
    musicManager.pause(false);
  }

  Video {
    id: video
    anchors.fill: parent
    source: assetPath + "videos/" + root.source
    onStatusChanged: {
      console.log("onStatusChanged", status, MediaPlayer.Loaded);
      if (status === MediaPlayer.Loaded) {
        play();
      }
    }
    onPlaybackStateChanged: {
      if (playbackState === MediaPlayer.StoppedState)
        onDone();
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: onDone();
  }
}
