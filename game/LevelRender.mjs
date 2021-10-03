import {CursorController} from "./level/Cursor.mjs";
import {RenderContextManager, RenderContextMode} from "./level/RenderContext.mjs";
import {initializeRenderQueue, fillRenderQueue} from "./RenderQueue.mjs";

export class Controller extends CursorController {
  constructor(canvas, params) {
    super(canvas, params);
    this.pathPrefix    = params.pathPrefix;
    this.level         = params.level;
    this.rootPath      = params.rootPath;
    this.preRenderPath = "file:///" + this.level.preRenderPath;
    this.frameCount = 0;
    this.initializeTilemap();
  }

  initializeRenderObjects() {
    this.renderObjects = initializeRenderQueue(this.mapSize);
    this.renderObjects = fillRenderQueue(this.renderObjects, this.level, this.optionViewAll);
  }

  render() {
    this.initializeTilemap();
    this.initializeRenderObjects();
    this.playerPosition = this.level.player ? this.level.player.position : Qt.point(-1, -1);
    this.clear();
    this.renderTilemap();
    this.renderLights();
    this.renderZones();
    this.contextManager = new RenderContextManager(this);
    this.eachCase(this.renderCoordinates.bind(this));
    this.renderVisualEffects();
    this.renderRoofs();
    this.contextManager.finalize();
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
      for (var y = 0 ; y < this.mapSize.height; ++y)
        callback(x, y);
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
        this.contextManager.playerRendered = true;
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
    this.contextManager.requireMode(RenderContextMode.ClipAroundPlayer);
    super.renderRoofs();
  }

  renderWall(tile, x, y) {
    const offset = this.getPointFor(x, y);

    if (this.shouldRender(offset.x, offset.y, this.wallSize.width, this.wallSize.height)) {
      this.contextManager.requireMode(RenderContextMode.ClipAroundPlayer);
      this.renderImage(this.pathPrefix + tile.image, offset, this.wallSize.width, this.wallSize.height, tile.clippedRect);
    }
  }

  getAdjustedOffsetFor(sprite) {
    return this.level.getAdjustedOffsetFor(sprite);
  }

  renderDynamicObject(sprite) {
    const offset      = this.getAdjustedOffsetFor(sprite);
    const clippedRect = sprite.getClippedRect();

    try {
      if (this.shouldRender(offset.x, offset.y, clippedRect.width, clippedRect.height)) {
        this.contextManager.requireMode(RenderContextMode.Normal);
        this.context.drawImage(
          this.pathPrefix + sprite.spriteSource,
          clippedRect.x, clippedRect.y, clippedRect.width, clippedRect.height,
          offset.x, offset.y, clippedRect.width, clippedRect.height
        );
        return true;
      }
    } catch (err) {
      console.log("LevelRender: cannot render", sprite.getObjectType(), ':', sprite.objectName, ": src=", this.pathPrefix + sprite.spriteSource, ", animation=" + sprite.getAnimation(), "at", offset);
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
