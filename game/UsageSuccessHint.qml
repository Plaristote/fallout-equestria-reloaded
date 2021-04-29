import QtQuick 2.15
import "./Interaction.mjs" as Interaction

Text {
  property QtObject levelController
  property QtObject target
  property var targetTile

  font.family: application.consoleFontName
  font.pointSize: 13
  color: "white"
  x: parent.mouseX + 30
  y: parent.mouseY + 5 - height / 2
  onEnabledChanged: refreshHint()
  onTargetChanged: refreshHint()
  onTargetTileChanged: refreshHint()

  function refreshHint() {
    var amount = null;

    if (levelController.mouseMode) {
      const item   = levelController.activeItem;
      const weapon = item && item.itemType === "weapon";

      switch (levelController.targetMode) {
      case Interaction.TargetMode.Character:
        if (target && target.getObjectType() === "Character")
          amount = item.getUseSuccessRate(target);
        break ;
      case Interaction.TargetMode.Zone:
        if (targetTile && targetTile.length === 2)
          amount = item.getUseAtSuccessRate(targetTile[0], targetTile[1]);
        break ;
      default:
        break ;
      }
    }
    if (amount !== null)
      text = (amount > 0 ? amount : 'X') + '%';
    else
      text = "";
  }
}
