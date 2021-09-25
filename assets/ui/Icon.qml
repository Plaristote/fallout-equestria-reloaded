import QtQuick 2.15
import QtGraphicalEffects 1.0

Image {
  property string name
  property string objectType

  source: `${assetPath}icons/ui/${name}.png`
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

  Colorize {
    anchors.fill: parent
    source: parent
    lightness: 1
  }
}
