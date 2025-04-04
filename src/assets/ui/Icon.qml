import QtQuick 2.15
import QtQuick.Effects

Image {
  id: self
  property string name
  property string objectType
  property real lightness: 1

  source: `file:assets/icons/ui/${name}.png`
  onObjectTypeChanged: name = getNameFromObjectType()

  function getNameFromObjectType() {
    switch (objectType) {
    case "Character":
      return "character";
    case "Doorway":
      return "door";
    case "InventoryItem":
      return "item";
    case "StorageObject":
      return "chest";
    case "ObjectGroup":
      return "group";
    }
    return "object";
  }

  layer.enabled: true
  layer.effect: MultiEffect { brightness: self.lightness }
}
