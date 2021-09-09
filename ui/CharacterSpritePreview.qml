import QtQuick 2.15
import Game 1.0 as Game;

Item {
  property QtObject character
  property string   animationName: "idle"
  property bool     animationRepeat: false

  Component.onCompleted: if (character) { initCharacter() }
  onCharacterChanged: initCharacter()

  function initCharacter() {
    var armor = character.inventory.getEquippedItem("armor");

    if (armor) {
      model.inventory.addItemOfType(armor.itemType);
      armor = model.inventory.getItemOfType(armor.itemType);
      model.inventory.equipItem(armor, "armor");
    }
    model.updateSpriteSheet();
    model.setAnimation(animationName);
  }

  Game.Character {
    id: model
    statistics: character.statistics
    onAnimationFinished: model.setAnimation(animationRepeat ? animationName : "idle")
  }

  Connections {
    target: model.statistics
    function onRaceChanged()      { initCharacter(); }
    function onFaceThemeChanged() { initCharacter(); }
    function onHairThemeChanged() { initCharacter(); }
    function onFaceColorChanged() { initCharacter(); }
    function onHairColorChanged() { initCharacter(); }
  }

  Timer {
    interval: 1000
    running: true
    repeat: true
    onTriggered: {
      const directions = ["up", "left", "down", "right"];
      const index = directions.indexOf(model.orientation);
      const nextIndex = index + 1 >= directions.length ? 0 : index + 1;

      model.orientation = directions[nextIndex];
    }
  }

  Image {
    anchors.centerIn: parent
    source: "file://" + model.spriteSource
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
