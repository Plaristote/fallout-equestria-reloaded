import {CursorController} from "./level/Cursor.mjs";
import {initializeRenderQueue, fillRenderQueue} from "./RenderQueue.mjs";

export class Controller extends CursorController {
  constructor(canvas, params) {
    super(canvas, params);
    this.pathPrefix = params.pathPrefix
    this.tilemap  = params.tilemap;
    this.level    = params.level;
    this.rootPath = params.rootPath;
    this.preRenderPath = "file:///" + this.level.preRenderPath;
    this.mapSize  = this.tilemap.mapSize;
    this.tileSize = this.tilemap.tileSize;
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
    super.render();
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
      return "qrc:/assets/tilesets/zones.png";
    return null;
  }

  renderZones() {
    for (var i = 0 ; i < this.tilemap.zones.length ; ++i) {
      const zone = this.tilemap.zones[i];
      const zoneTexture = this.getTextureForZone(zone);

      if (zoneTexture !== null)
      {
        for (var ii = 0 ; ii < zone.getPositionCount() ; ++ii)
        {
          const position = zone.getPositionAt(ii);
          const tile = this.layers.ground.getTile(position.x, position.y);

          if (tile)
            this.renderImage(zoneTexture, tile.renderPosition, this.tileSize.width, this.tileSize.height, zone.clippedRect);
        }
      }
    }
  }

  renderGroundLayer(layer, path) {
    const layerRect  = layer.getRenderedRect();
    const renderRect = this.layers.ground.getRenderedRect();
    const size       = { width: layerRect.width, height: layerRect.height };
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
    offset.x -= layer.offset.x;
    offset.y -= layer.offset.y;
    //console.log(`Drawing floor ${layer.name} at`, `{x:${position.x},y:${position.y}}`, "rect", this.origin.x, this.origin.y, width, height);
    //console.log("-> offset", offset.x, '/', offset.y);
    try {
      this.context.drawImage(
        this.preRenderPath + path,
        offset.x,   offset.y,   width, height,
        position.x, position.y, width, height
      );
    } catch (err) {
      console.log("renderGroundLayer(", layer, path, "): render error:", err);
    }
  }

  renderTilemap() {
    this.renderGroundLayer(this.layers.ground, "tilemap.png");
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
    if (layer.name.startsWith("floor_")) {
      if (this.shouldRenderLayer(layer))
        this.renderGroundLayer(layer, "roof_" + layer.name + ".png");
    }
    else
      this.renderLayer(layer, "roof_" + layer.name + ".png");
  }

  renderLightLayer(layer) {
    this.renderLayer(layer, "lights_" + layer.name + ".png");
  }

  shouldRenderLayer(layer) {
    const renderRect = layer.getRenderedRect();

    return this.shouldRender(renderRect.x, renderRect.y, renderRect.width, renderRect.height);
  }

  renderLayer(layer, path) {
    if (this.shouldRenderLayer(layer)) {
      if (layer.prerendered)
        this.renderAsPrerenderedLayer(layer, path, layer.getRenderedRect());
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
};
