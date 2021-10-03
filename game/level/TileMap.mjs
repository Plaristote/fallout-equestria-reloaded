import {CanvasRenderer} from "./CanvasRenderer.mjs";

export class TileMapRenderer extends CanvasRenderer {
  initializeTilemap() {
    this.tilemap  = this.level.tilemap;
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
        this.preRenderPath + `floor${this.level.currentFloor}_${path}`,
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

  renderRoofs() {
    const player = this.level.player;
    const playerRoof = player ? this.level.getRoofFor(this.level.player) : null;

    for (var i = 0 ; i < this.tilemap.roofs.length ; ++i) {
      const layer = this.tilemap.roofs[i];

      layer.visible = playerRoof !== layer;
      if (layer.visible)
        this.renderRoofLayer(layer);
    }
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
      this.preRenderPath + `floor${this.level.currentFloor}_${path}`,
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
      this.renderImage(this.pathPrefix + tile.image, offset, this.wallSize.width, this.wallSize.height, tile.clippedRect);
    }
  }
}
