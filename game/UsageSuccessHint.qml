import QtQuick 2.15

Text {
  property QtObject levelController
  property QtObject target

  font.family: application.consoleFontName
  font.pointSize: 13
  color: "white"
  x: parent.mouseX + 30
  y: parent.mouseY + 5 - height / 2
  onEnabledChanged: refreshHint()
  onTargetChanged: refreshHint()

  function refreshHint() {
    if (levelController.activeItem && target && target.getObjectType() === "Character") {
      const item   = levelController.activeItem;
      const weapon = item.itemType === "wepaon";

      if (weapon && target === levelController.player)
        text = "";
      else {
        const amount = item.getUseSuccessRate(target);

        text = (amount > 0 ? amount : 'X') + "%";
      }
    }
    else
      text = "";
  }
}
