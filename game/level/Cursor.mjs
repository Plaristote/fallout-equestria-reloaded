import {CanvasRenderer} from "./CanvasRenderer.mjs";
import {MouseMode, TargetMode} from "../Interaction.mjs";

export class CursorController extends CanvasRenderer {
  constructor(canvas, params) {
    super(canvas);
    this.level = params.level;
  }

  render() {
    if (this.level.mouseMode === MouseMode.Movement) // if cursor is movement mode
      this.renderMoveCursor();
    if (this.level.mouseMode === MouseMode.Target && this.level.targetMode == TargetMode.Zone)
      this.renderZoneCursor();
  }

  renderZoneCursor() {
    if (this.level.player && this.level.isCharacterTurn(this.level.player)) {
      const coordinates = this.canvas.hoverTile;
      const item = this.level.activeItem;

      if (item && coordinates && coordinates.length === 2) {
        const from = { x: coordinates[0] - item.zoneSize, y: coordinates[1] - item.zoneSize };
        const to   = { x: coordinates[0] + item.zoneSize, y: coordinates[1] + item.zoneSize };

        for (var x = from.x ; x <= to.x ; ++x) {
          for (var y = from.y ; y <= to.y ; ++y) {
            const tile = this.layers.ground.getTile(x, y);

            if (tile)
              this.renderImage(`qrc:/assets/ui/cursors/target-tile.png`, tile.renderPosition, this.tileSize.width, this.tileSize.height);
          }
        }
      }
    }
  }

  renderCombatMoveCursor() {
    if (this.level.player && this.level.isCharacterTurn(this.level.player)) {
      const coordinates = this.canvas.hoverTile;

      if (coordinates && coordinates.length === 2) {
        const path = this.level.previewPathTo(coordinates[0], coordinates[1]);
        const maxLength = Math.min(this.level.player.actionPoints, path.length);

        for (var i = 0 ; i < maxLength ; ++i) {
          const tile = this.layers.ground.getTile(path[i].x, path[i].y);
          const lastTile = i + 1 === maxLength;
          const texture = lastTile ? "move-tile.png" : "move-tile-unavailable.png";

          if (tile) {
            this.renderImage(`qrc:/assets/ui/cursors/${texture}`, tile.renderPosition, this.tileSize.width, this.tileSize.height);
            if (lastTile) {
              this.context.font = "bold 20px sans-serif";
              this.context.fillStyle = "#FFFFFF";
              this.context.fillText(`${maxLength}`, tile.renderPosition.x + this.tileSize.width / 2 - 10, tile.renderPosition.y + this.tileSize.height / 2);
              this.context.strokeStyle = "#000000";
              this.context.strokeText(`${maxLength}`, tile.renderPosition.x + this.tileSize.width / 2 - 10, tile.renderPosition.y + this.tileSize.height / 2);
            }
          }
        }
        this.canvas.hoverTileHintVisible = !(maxLength !== path.length || path.length === 0);
        return true;
      }
    }
    return false;
  }

  renderMoveCursor() {
    if (!this.renderCombatMoveCursor())
    {
      const coordinates = this.canvas.hoverTile;

      if (coordinates && coordinates.length === 2) {
        const tile = this.layers.ground.getTile(coordinates[0], coordinates[1]);

        if (tile)
        {
          const occupied = this.level.grid.isOccupied(coordinates[0], coordinates[1]);
          const texture  = !occupied ? "move-tile.png" : "move-tile-unavailable.png";

          this.canvas.hoverTileHintVisible = true;
          this.renderImage(`qrc:/assets/ui/cursors/${texture}`, tile.renderPosition, this.tileSize.width, this.tileSize.height);
          return ;
        }
      }
      this.canvas.hoverTileHintVisible = false;
    }
  }

  getHoveredCase(posX, posY) {
    for (var x = 0 ; x < this.mapSize.width; ++x) {
      for (var y = 0 ; y < this.mapSize.height; ++y) {
        const pos = this.getPointFor(x, y);
        if (posX >= pos.x && posX <= pos.x + this.tileSize.width &&
            posY >= pos.y && posY <= pos.y + this.tileSize.height)
        {
          return [x, y];
        }
      }
    }
    return null;
  }

  onMouseClick(mouse, mouseX, mouseY) {
    if (!this.level.combat || this.level.isPlayerTurn) {
      mouseX -= this.canvas.origin.x;
      mouseY -= this.canvas.origin.y;
      if (this.level.mouseMode === MouseMode.Movement || (this.level.mouseMode === MouseMode.Target && this.level.targetMode === TargetMode.Zone))
        this.onCaseClick(mouseX, mouseY);
      else if (this.level.mouseMode === MouseMode.Target || this.level.mouseMode === MouseMode.Interaction)
        this.onObjectClick(mouseX, mouseY);
    }
  }

  onCaseClick(mouseX, mouseY) {
    const coords = this.getHoveredCase(mouseX, mouseY);

    if (coords !== null)
      this.level.tileClicked(coords[0], coords[1]);
  }

  onObjectClick(mouseX, mouseY) {
    const object = this.level.getObjectAt(mouseX, mouseY);

    if (object !== null)
      this.level.objectClicked(object);
  }
}
