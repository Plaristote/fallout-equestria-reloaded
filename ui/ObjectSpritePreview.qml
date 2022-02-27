import QtQuick 2.15
import Game 1.0 as Game;

Item {
  property QtObject dynamicObject

  onDynamicObjectChanged: updateDynamicObject()
  Component.onCompleted:  updateDynamicObject()
  height: image.height
  width:  image.width

  function updateDynamicObject() {
    model.orientation = dynamicObject.orientation;
    model.spriteName  = dynamicObject.spriteName;
    model.setAnimation(dynamicObject.getAnimation());
  }

  Game.DynamicObject {
    id: model
  }

  Timer {
    interval: 1000
    running: dynamicObject.orientation !== ""
    repeat: true
    onTriggered: {
      const directions = ["up", "left", "down", "right"];
      const index = directions.indexOf(model.orientation);
      const nextIndex = index + 1 >= directions.length ? 0 : index + 1;

      model.orientation = directions[nextIndex];
    }
  }

  Image {
    id: image
    anchors.centerIn: parent
    source: model.spriteSource.length > 0 ? "file://" + model.spriteSource : ""
    height: model.clippedRect.height
    width:  model.clippedRect.width
    sourceClipRect: model.clippedRect
    Timer {
      interval: 100
      running: true
      repeat: true
      onTriggered: model.update(interval)
    }
  }
}
