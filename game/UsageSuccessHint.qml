import QtQuick 2.15
import "./Interaction.mjs" as Interaction

Text {
  property QtObject levelController
  property QtObject target
  property var targetTile

  font.family: application.consoleFontName
  font.pointSize: application.consoleFont.bigSize * (13/12)
  color: "white"
  style: Text.Outline
  styleColor: "black"
  x: parent.mouseX + 30
  y: parent.mouseY + 5 - height / 2
  onEnabledChanged: refreshHint()
  onTargetChanged: refreshHint()
  onTargetTileChanged: refreshHint()

  function refreshHint() {
    switch (levelController.mouseMode) {
    case 1:
      targetHint();
      break ;
    case 2:
      successRateHint();
      break ;
    default:
      text = "";
      break ;
    }
  }

  function targetHint() {
    if (target)
      text = target.getHint();
    else
      text = "";
  }

  function successRateHint() {
    var   amount = null;
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
    if (amount !== null)
      text = (amount > 0 ? amount : 'X') + '%';
    else
      text = "";
  }
}
