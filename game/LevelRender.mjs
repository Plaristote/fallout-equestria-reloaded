export class Controller {
  constructor(canvas, params) {
    this.pathPrefix = params.pathPrefix
    this.canvas   = canvas;
    this.context  = canvas.getContext("2d");
    this.tilemap  = params.tilemap;
    this.level    = params.level;
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
    this.renderObjects = [];
    for (var i = 0 ; i < this.level.dynamicObjects.length ; ++i)
      this.renderObjects.push(this.level.dynamicObjects[i]);
  }

  render() {
    this.initializeRenderObjects();
    this.playerPosition = this.level.player ? this.level.player.position : Qt.point(-1, -1);
    this.clear();
    this.eachCase(this.renderTile.bind(this));
    this.renderZones();
    this.eachCase(this.renderCoordinates.bind(this));
    this.renderRoofs();
    this.frameCount++;
  }

  renderRoofs() {
    this.startClipAroundPlayer();
    this.eachCase(this.renderRoof.bind(this));
    this.stopClipAroundPlayer();
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

  renderTile(x, y) {
    const tile = this.layers.ground.getTile(x, y);

    if (tile)
      this.renderImage(this.pathPrefix + tile.image, tile.renderPosition, this.tileSize.width, this.tileSize.height, tile.clippedRect);
  }

  renderCoordinates(x, y) {
    const wall = this.layers.walls.getTile(x, y);

    if (wall)
      return this.renderWall(wall, x, y);
    for (var i = 0 ; i < this.renderObjects.length ; ++i) {
      const objectPosition = this.renderObjects[i].getPosition();

      if (objectPosition.x === x && objectPosition.y === y) {
        this.renderSprite(this.renderObjects[i]);
        this.renderObjects.splice(i, 1);
        break ;
      }
    }
  }

  renderRoof(x, y) {
    const player = this.level.player;

    for (var i = 0 ; i < this.tilemap.roofs.length ; ++i) {
      const layer = this.tilemap.roofs[i];

      if (player) {
        const tile = layer.getTile(player.position.x, player.position.y);

        if (tile)
          layer.visible = false;
        else
          layer.visible = true;
      }
      if (layer.visible) {
        this.eachCase((x, y) => {
          const tile = this.tilemap.roofs[i].getTile(x, y);
          if (tile)
            this.renderImage(this.pathPrefix + tile.image, tile.renderPosition, this.tileSize.width, this.tileSize.height, tile.clippedRect);
        });
      }
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
    const sprite      = this.level.player;

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
    const offset = sprite.getSpritePosition();

    if (sprite.renderOnTile()) {
      const clippedRect = sprite.getClippedRect();
      const extraHeight = clippedRect.height - this.tileSize.height;

      offset.y -= extraHeight;
      offset.x += this.tileSize.width / 2 - clippedRect.width / 2;
    }
    return offset;
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
    return {
      minX: -(this.mapSize.width * this.tileSize.width / 2) + this.canvas.width / 2,
      maxY: this.canvas.height / 2,
      maxX: (this.mapSize.height * this.tileSize.width / 2) + this.canvas.width / 2,
      minY: -(this.mapSize.width * this.tileSize.height / 2) - this.canvas.height / 2
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

  getHoveredObject(posX, posY) {
    for (var i = 0 ; i < this.level.dynamicObjects.length ; ++i) {
      const sprite = this.level.dynamicObjects[i];
      const offset = this.getAdjustedOffsetFor(sprite);
      const clippedRect = sprite.getClippedRect();

      if (posX >= offset.x && posX <= offset.x + clippedRect.width &&
          posY >= offset.y && posY <= offset.y + clippedRect.height)
        return sprite;
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
    const object = this.getHoveredObject(mouseX, mouseY);

    if (object !== null)
      this.level.objectClicked(object);
  }
};
