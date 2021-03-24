export class Controller {
  constructor(canvas, params) {
    this.pathPrefix = params.pathPrefix
    this.canvas   = canvas;
    this.context  = canvas.getContext("2d");
    this.tilemap  = params.tilemap;
    this.level    = params.level;
    this.rootPath = params.rootPath;
    this.mapSize  = this.tilemap.mapSize
    this.tileSize = this.tilemap.tileSize
    this.wallSize = { width: this.tileSize.width, height: this.tileSize.height * 3 };
    this.layers   = {
      ground: this.tilemap.getLayer("ground"),
      walls:  this.tilemap.getLayer("walls")
    };
    this.frameCount = 0;
  }

  get origin() {
    return this.canvas.origin;
  }

  initializeRenderObjects() {
    const _detectedCharacters = this.level.visibleCharacters;
    const detectedCharacters = [];

    for (var i = 0 ; i < _detectedCharacters.length ; ++i)
      detectedCharacters.push(_detectedCharacters[i].objectName);
    this.renderObjects = [];
    for (var x = 0 ; x < this.mapSize.width ; ++x) {
      this.renderObjects[x] = [];
      for (var y = 0 ; y < this.mapSize.height ; ++y)
        this.renderObjects[x][y] = [];
    }
    for (var i = 0 ; i < this.level.dynamicObjects.length ; ++i) {
      const object = this.level.dynamicObjects[i];
      const position = object.getPosition();
      const isUndetected = object.getObjectType() === "Character" && detectedCharacters.indexOf(object.objectName) < 0;
      const isValid = position.x >= 0 && position.y >= 0;

      if (isUndetected || !isValid)
        continue ;
      this.renderObjects[position.x][position.y].push(object);
      this.renderObjects[position.x][position.y].sort(function(a, b) {
        return a.getZIndex() < b.getZIndex();
      });
    }
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
    if (this.level.mouseMode === 0) // if cursor is movement mode
      this.renderMoveCursor();
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
    if (zone.type === "exit" || (this.canvas.editorObject && this.canvas.editorObject.controlZone === zone))
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
      this.rootPath + "_tilemap.png",
      offset.x,   offset.y,   width, height,
      position.x, position.y, width, height
    );
  }

  renderMoveCursor() {
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

  renderCoordinates(x, y) {
    const wall = this.layers.walls.getTile(x, y);

    if (wall)
      return this.renderWall(wall, x, y);
    this.renderObjects[x][y].forEach(this.renderSprite.bind(this));
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
    this.renderLayer(layer, "_roof_" + layer.name + ".png");
  }

  renderLightLayer(layer) {
    this.renderLayer(layer, "_lights_" + layer.name + ".png");
  }

  renderLayer(layer, path) {
    const renderRect = layer.getRenderedRect();

    if (this.shouldRender(renderRect.x, renderRect.y, renderRect.width, renderRect.height)) {

      this.context.drawImage(
        this.rootPath + path,
        renderRect.x, renderRect.y, renderRect.width, renderRect.height
      );
    }
  }

  renderWall(tile, x, y) {
    if (this.renderAfterPlayer)
      this.startClipAroundPlayer();
    this.renderImage(this.pathPrefix + tile.image, this.getPointFor(x, y), this.wallSize.width, this.wallSize.height, tile.clippedRect);
    if (this.renderAfterPlayer)
      this.stopClipAroundPlayer();
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

  renderSprite(sprite) {
    const offset      = this.getAdjustedOffsetFor(sprite);
    const clippedRect = sprite.getClippedRect();

    if (this.shouldRender(offset.x, offset.y, clippedRect.width, clippedRect.height)) {
      if (sprite.getShadowSource() !== "")
      {
        this.context.drawImage(
          this.pathPrefix + sprite.getShadowSource(),
          //clippedRect.x, clippedRect.y, clippedRect.width, clippedRect.height,
          offset.x, offset.y + 3, clippedRect.width, clippedRect.height
        );
      }
      this.context.drawImage(
        this.pathPrefix + sprite.getSpriteSource(),
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
    this.context.clearRect(-this.origin.x, -this.origin.y, this.canvas.width * 100, this.canvas.height * 100);
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
      switch (this.level.mouseMode) {
        case 0:
          this.onMovementClick(mouseX, mouseY);
          break ;
        default:
          this.onObjectClick(mouseX, mouseY);
          break ;
      }
    }
  }

  onMovementClick(mouseX, mouseY) {
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
