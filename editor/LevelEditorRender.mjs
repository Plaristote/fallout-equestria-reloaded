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

  onObjectClick(mouseX, mouseY) {
    const object = this.getHoveredObject(mouseX, mouseY);

    if (object)
      this.canvas.pickedObject(object);
  }

  onMovementClick(mouseX, mouseY) {
    const coords = this.getHoveredCase(mouseX, mouseY);

    if (this.canvas.editingZone)
      this.canvas.toggleZoneTile(coords[0], coords[1]);
    else
      this.canvas.pickedTile(coords[0], coords[1]);
  }
};
