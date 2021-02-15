import {Controller} from "../game/LevelRender.mjs";

export class EditorController extends Controller {
  renderRoofs() {
    if (this.canvas.renderRoofs)
      super.renderRoofs();
  }

  renderWall(wall, x, y) {
    if (this.canvas.renderWalls)
      super.renderWall(wall, x, y);
  }

  onMouseClick(mouse, mouseX, mouseY)
  {
    console.log("On mouze click, editing zone?", this.canvas.editingZone);
    if (this.canvas.editingZone) {
      const coords = this.getHoveredCase(mouseX, mouseY);

      if (coords !== null)
        this.canvas.toggleZoneTile(coords[0], coords[1]);
      return ;
    }
    super.onMouseClicked(mouse, mouseX, mouseY);
  }
};
