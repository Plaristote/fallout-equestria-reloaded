import {MouseMode, TargetMode} from "./Interaction.mjs";
import {initializeRenderQueue, fillRenderQueue} from "./RenderQueue.mjs";

export class Controller {
  constructor(canvas, params) {
    this.pathPrefix = params.pathPrefix
    this.canvas   = canvas;
    this.context  = canvas.getContext("2d");
    this.tilemap  = params.tilemap;
    this.level    = params.level;
    this.rootPath = params.rootPath;
    this.preRenderPath = "file:///" + this.level.preRenderPath;
    this.mapSize  = this.tilemap.mapSize
    this.tileSize = this.tilemap.tileSize
    this.wallSize = { width: this.tileSize.width, height: this.tileSize.height * 3 };
    this.layers   = {
      ground: this.tilemap.getLayer("ground"),
      blocks: this.tilemap.getLayer("blocks"),
      vwalls: this.tilemap.getLayer("walls-v"),
      hwalls: this.tilemap.getLayer("walls-h"),
      misc:   this.tilemap.getLayer("misc")
    };
    this.frameCount = 0;
  }

  get origin() {
    return this.canvas.origin;
  }

  isObjectVisible(object, detectedCharacters) {
    if (object.getObjectType() === "Character")
      return detectedCharacters.indexOf(object.objectName) >= 0;
    return !object.hidden;
  }

  initializeRenderObjects() {
    this.renderObjects = initializeRenderQueue(this.mapSize);
    this.renderObjects = fillRenderQueue(this.renderObjects, this.level, this.optionViewAll);
  }

  render() {
    this.initializeRenderObjects();
    this.playerPosition = this.level.player ? this.level.player.position : Qt.point(-1, -1);
    this.clear();
    this.renderTilemap();
    this.renderLights();
    this.renderZones();
    this.eachCase(this.renderCoordinates.bind(this));
    this.renderVisualEffects();
    this.renderRoofs();
    if (this.level.mouseMode === MouseMode.Movement) // if cursor is movement mode
      this.renderMoveCursor();
    if (this.level.mouseMode === MouseMode.Target && this.level.targetMode == TargetMode.Zone)
      this.renderZoneCursor();
    this.frameCount++;
  }

  renderVisualEffects() {
    const visualEffects = this.level.visualEffects;

    for (var i = 0 ; i < visualEffects.length ; ++i)
      this.renderSprite(visualEffects[i]);
  }

  eachCase(callback) {
    this.renderAfterPlayer = false;
    for (var x = 0 ; x < this.mapSize.width; ++x) {
      for (var y = 0 ; y < this.mapSize.height; ++y) {
        callback(x, y);
        if (this.playerPosition.x === x && this.playerPosition.y === y)
          this.renderAfterPlayer = true;
      }
    }
  }

  getTextureForZone(zone) {
    if (zone.type === "exit")
      return "../assets/tilesets/zones.png";
    return null;
  }

  renderZones() {
    for (var i = 0 ; i < this.tilemap.zones.length ; ++i) {
      const zone = this.tilemap.zones[i];
      const zoneTexture = this.getTextureForZone(zone);

      if (zoneTexture !== null)
      {
        for (var ii = 0 ; ii < this.tilemap.zones[i].getPositionCount() ; ++ii)
        {
          const position = this.tilemap.zones[i].getPositionAt(ii);
          const tile = this.layers.ground.getTile(position.x, position.y);

          this.renderImage(zoneTexture, tile.renderPosition, this.tileSize.width, this.tileSize.height, zone.clippedRect);
        }
      }
    }
  }

  renderTilemap() {
    const renderRect = this.layers.ground.getRenderedRect();
    const size       = { width: renderRect.width, height: renderRect.height };
    const position   = {x: -this.origin.x, y: -this.origin.y};
    const offset     = {x: -renderRect.x - this.origin.x, y: renderRect.y -this.origin.y};
    var   width      = offset.x + this.canvas.width > size.width   ? size.width  - offset.x : this.canvas.width;
    var   height     = offset.y + this.canvas.height > size.height ? size.height - offset.y : this.canvas.height;

    if (offset.x < 0)
    {
      width      += offset.x;
      position.x -= offset.x;
      offset.x    = 0;
    }
    if (offset.y < 0)
    {
      height     += offset.y;
      position.y -= offset.y;
      offset.y    = 0;
    }
    //console.log("Drawing tilemap at", JSON.stringify(position), "rect", this.origin.x, this.origin.y, width, height);
    //console.log("-> offset", offset.x, '/', offset.y);
    this.context.drawImage(
      this.preRenderPath + "tilemap.png",
      offset.x,   offset.y,   width, height,
      position.x, position.y, width, height
    );
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
              this.renderImage(`../assets/ui/cursors/target-tile.png`, tile.renderPosition, this.tileSize.width, this.tileSize.height);
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
            this.renderImage(`../assets/ui/cursors/${texture}`, tile.renderPosition, this.tileSize.width, this.tileSize.height);
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
          this.renderImage(`../assets/ui/cursors/${texture}`, tile.renderPosition, this.tileSize.width, this.tileSize.height);
          return ;
        }
      }
      this.canvas.hoverTileHintVisible = false;
    }
  }

  renderCoordinates(x, y) {
    const block = this.layers.blocks ? this.layers.blocks.getTile(x, y) : null;

    if (block)
      this.renderWall(block, x, y);
    {
      const vwall = this.layers.vwalls ? this.layers.vwalls.getTile(x, y) : null;
      const hwall = this.layers.hwalls ? this.layers.hwalls.getTile(x, y) : null;

      this.renderMisc(x, y);
      this.renderObjects[x][y].forEach(this.renderDynamicObject.bind(this));
      if (this.playerPosition.x === x && this.playerPosition.y === y)
        this.renderAfterPlayer = true;
      if (vwall)
        this.renderWall(vwall, x, y);
      if (hwall)
        this.renderWall(hwall, x, y);
    }
  }

  renderMisc(x, y) {
    if (this.layers.misc) {
      const misc = this.layers.misc.getTile(x, y);

      if (misc)
        this.renderWall(misc, x, y);
    }
  }

  renderRoofs() {
    const player = this.level.player;
    const playerRoof = player ? this.level.getRoofFor(this.level.player) : null;

    this.startClipAroundPlayer();
    for (var i = 0 ; i < this.tilemap.roofs.length ; ++i) {
      const layer = this.tilemap.roofs[i];

      layer.visible = playerRoof !== layer;
      if (layer.visible)
        this.renderRoofLayer(layer);
    }
    this.stopClipAroundPlayer();
  }

  renderLights() {
    for (var i = 0 ; i < this.tilemap.lights.length ; ++i) {
      const layer = this.tilemap.lights[i];

      if (layer.visible)
        this.renderLightLayer(layer);
    }
  }

  renderRoofLayer(layer) {
    this.renderLayer(layer, "roof_" + layer.name + ".png");
  }

  renderLightLayer(layer) {
    this.renderLayer(layer, "lights_" + layer.name + ".png");
  }

  renderLayer(layer, path) {
    const renderRect = layer.getRenderedRect();

    if (this.shouldRender(renderRect.x, renderRect.y, renderRect.width, renderRect.height)) {
      if (layer.prerendered)
        this.renderAsPrerenderedLayer(layer, path, renderRect);
      else
        this.renderAsDynamicLayer(layer);
    }
  }

  renderAsPrerenderedLayer(layer, path, renderRect) {
    this.context.drawImage(
      this.preRenderPath + path,
      renderRect.x, renderRect.y, renderRect.width, renderRect.height
    );
  }

  renderAsDynamicLayer(layer) {
    this.eachCase((function(x, y) {
      const tile = layer.getTile(x, y);

      if (tile)
        this.renderImage(this.pathPrefix + tile.image, tile.renderPosition, this.tileSize.width, this.tileSize.height, tile.clippedRect);
    }).bind(this));
  }

  renderWall(tile, x, y) {
    const offset = this.getPointFor(x, y);

    if (this.shouldRender(offset.x, offset.y, this.wallSize.width, this.wallSize.height)) {
      if (this.renderAfterPlayer)
        this.startClipAroundPlayer();
      this.renderImage(this.pathPrefix + tile.image, offset, this.wallSize.width, this.wallSize.height, tile.clippedRect);
      if (this.renderAfterPlayer)
        this.stopClipAroundPlayer();
    }
  }

  startClipAroundPlayer() {
    const sprite = this.level.player;

    if (sprite)
    {
      const offset      = sprite.getSpritePosition();
      const clippedRect = sprite.getClippedRect();
      const extraHeight = clippedRect.height - this.tileSize.height;

      offset.y -= extraHeight;
      offset.x += this.tileSize.width / 2 - clippedRect.width / 2;
      this.context.save();
      this.context.beginPath();
      this.context.arc(offset.x + this.tileSize.width / 2 - clippedRect.width / 4, offset.y + this.tileSize.height / 2, clippedRect.width, 0, Math.PI * 2, false);
      this.context.rect(-this.canvas.origin.x + this.canvas.width, -this.canvas.origin.y, - this.canvas.width, this.canvas.height);
      this.context.clip();
    }
  }

  stopClipAroundPlayer() {
    this.context.restore();
  }

  getAdjustedOffsetFor(sprite) {
    return this.level.getAdjustedOffsetFor(sprite);
  }

  renderDynamicObject(sprite) {
    const offset      = this.getAdjustedOffsetFor(sprite);
    const clippedRect = sprite.getClippedRect();

    try {
      if (this.shouldRender(offset.x, offset.y, clippedRect.width, clippedRect.height)) {
        this.context.drawImage(
          this.pathPrefix + sprite.spriteSource,
          clippedRect.x, clippedRect.y, clippedRect.width, clippedRect.height,
          offset.x, offset.y, clippedRect.width, clippedRect.height
        );
/*
                if (sprite.getObjectType() === "Character") {
                  const vision_score = this.level.grid.getVisionQuality(this.level.player.position.x, this.level.player.position.y, sprite.position.x, sprite.position.y);
                  this.context.font = "bold 20px sans-serif";
                  this.context.fillStyle = "#FFFFFF";
                  this.context.fillText(`${vision_score}%`, offset.x, offset.y);
                  this.context.strokeStyle = "#000000";
                  this.context.strokeText(`${vision_score}%`, offset.x, offset.y);
                }
*/
        return true;
      }
    } catch (err) {
      console.log("LevelRender: cannot render", sprite.getObjectType(), ':', sprite.objectName, ": src=", this.pathPrefix + sprite.spriteSource, ", animation=" + sprite.getAnimation());
    }
    return false;
  }

  renderSprite(sprite) {
    const offset      = sprite.spritePosition;
    const clippedRect = sprite.clippedRect;

    if (this.shouldRender(offset.x, offset.y, clippedRect.width, clippedRect.height)) {
      this.context.drawImage(
        this.pathPrefix + sprite.spriteSource,
        clippedRect.x, clippedRect.y, clippedRect.width, clippedRect.height,
        offset.x, offset.y, clippedRect.width, clippedRect.height
      );
      return true;
    }
  }

  renderImage(source, tileOffset, width, height, clippedRect) {
    const extraHeight = height - this.tileSize.height;
    const offset  = { x: tileOffset.x, y: tileOffset.y - extraHeight };

    if (clippedRect === undefined)
      clippedRect = Qt.rect(0, 0, width, height);
    if (this.shouldRender(offset.x, offset.y, width, height)) {
      this.context.drawImage(
        source,
        clippedRect.x, clippedRect.y, clippedRect.width, clippedRect.height,
        offset.x, offset.y, width, height
      );
      return true;
    }
    return false;
  }

  getPointFor(x, y) {
    return Qt.point(
      x * this.tileSize.width  / 2 - y * this.tileSize.width  / 2,
      y * this.tileSize.height / 2 + x * this.tileSize.height / 2
    );
  }

  getLimits() {
    const rect = this.layers.ground.getRenderedRect();

    return {
      minX: -(this.mapSize.width * this.tileSize.width / 2) + this.canvas.width / 2,
      maxY: this.canvas.height / 2,
      maxX: (this.mapSize.height * this.tileSize.width / 2) + this.canvas.width / 2,
      minY: -rect.height + this.canvas.height / 2
    };
  }

  shouldRender(x, y, width, height)   {
    return x + width  >= -this.origin.x && x <= -this.origin.x + this.canvas.width
        && y + height >= -this.origin.y && y <= -this.origin.y + this.canvas.height;
  }

  clear() {
    this.context.clearRect(-this.origin.x, -this.origin.y, this.canvas.width * 10, this.canvas.height * 10);
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
};
