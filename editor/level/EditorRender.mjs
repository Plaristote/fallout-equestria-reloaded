import {Controller} from "../../game/LevelRender.mjs";

export class EditorController extends Controller {
  isObjectVisible() {
    return true;
  }

  getTextureForZone(zone) {
    const texture = super.getTextureForZone(zone);

    if (texture === null && this.canvas.editorObject && this.canvas.editorObject.controlZone === zone) {
      return "../assets/tilesets/zones.png";
    }
    return null;
  }

  renderRoofs() {
    if (this.canvas.renderRoofs)
      super.renderRoofs();
  }

  renderWall(wall, x, y) {
    if (this.canvas.renderWalls)
      super.renderWall(wall, x, y);
  }

  onObjectClick(mouseX, mouseY) {
    const object = this.level.getObjectAt(mouseX, mouseY);

    if (object)
      this.canvas.pickedObject(object);
  }

  onCaseClick(mouseX, mouseY) {
    const coords = this.getHoveredCase(mouseX, mouseY);

    if (this.canvas.editingZone)
      this.canvas.toggleZoneTile(coords[0], coords[1]);
    else
      this.canvas.pickedTile(coords[0], coords[1]);
  }
};
