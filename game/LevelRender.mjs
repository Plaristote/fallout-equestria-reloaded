export class Controller {
  constructor(canvas, params) {
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
  }

  get origin() {
    return this.canvas.origin;
  }

  render() {
    var totalRendered = 0;

    this.clear();
    this.eachCase(this.renderTile.bind(this));
    this.renderZones();
    this.eachCase(this.renderCoordinates.bind(this));
    this.eachCase(this.renderRoofs.bind(this));
    this.context.restore();
  }

  eachCase(callback) {
    for (var x = 0 ; x < this.mapSize.width; ++x) {
      for (var y = 0 ; y < this.mapSize.height; ++y) {
        callback(x, y);
      }
    }
  }

  renderZones() {
    for (var i = 0 ; i < this.tilemap.zones.length ; ++i) {
      const zone = this.tilemap.zones[i];
      if (zone.type == "exit")
      {
        for (var ii = 0 ; ii < this.tilemap.zones[i].getPositionCount() ; ++ii)
        {
          const position = this.tilemap.zones[i].getPositionAt(ii);
          const tile = this.layers.ground.getTile(position.x, position.y);

          this.renderImage("../assets/tilesets/zones.png", tile.renderPosition, this.tileSize.width, this.tileSize.height, zone.clippedRect);
        }
      }
    }
  }

  renderTile(x, y) {
    const tile = this.layers.ground.getTile(x, y);

    if (tile)
    {
      this.renderImage("../" + tile.image, tile.renderPosition, this.tileSize.width, this.tileSize.height, tile.clippedRect);
    }
  }

  renderCoordinates(x, y) {
    const wall = this.layers.walls.getTile(x, y);
    const dynamicObject = this.level.getOccupantAt(x, y);

    if (wall)
      this.renderWall(wall, x, y);
    else if (dynamicObject)
      this.renderSprite(dynamicObject);
  }

  renderRoofs(x, y) {
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
            this.renderImage("../" + tile.image, tile.renderPosition, this.tileSize.width, this.tileSize.height, tile.clippedRect);
        });
      }
    }
  }

  renderWall(tile, x, y) {
    this.renderImage("../" + tile.image, this.getPointFor(x, y), this.wallSize.width, this.wallSize.height, tile.clippedRect);
  }

  renderSprite(sprite) {
    const offset      = sprite.getSpritePosition();
    const clippedRect = sprite.getClippedRect();

    if (sprite.renderOnTile()) {
      const extraHeight = clippedRect.height - this.tileSize.height;

      offset.y -= extraHeight;
      offset.x += this.tileSize.width / 2 - clippedRect.width / 2;
    }

    if (this.shouldRender(offset.x, offset.y, clippedRect.width, clippedRect.height)) {
      if (sprite.getShadowSource() !== "")
      {
        this.context.drawImage(
          "../" + sprite.getShadowSource(),
          //clippedRect.x, clippedRect.y, clippedRect.width, clippedRect.height,
          offset.x, offset.y + 3, clippedRect.width, clippedRect.height
        );
      }
      this.context.drawImage(
        "../" + sprite.getSpriteSource(),
        clippedRect.x, clippedRect.y, clippedRect.width, clippedRect.height,
        offset.x, offset.y, clippedRect.width, clippedRect.height
      );
      if (sprite === this.level.player) {
        //console.log("PLAYER RENDERING");
        this.context.save();
        this.context.beginPath();
        this.context.arc(offset.x + this.tileSize.width / 2 - clippedRect.width / 4, offset.y + this.tileSize.height / 2, clippedRect.width, 0, Math.PI * 2, false);
        this.context.rect(-this.canvas.origin.x + this.canvas.width, -this.canvas.origin.y, - this.canvas.width, this.canvas.height);
        //this.context.closePath();
        //this.context.globalCompositeOperation = "xor";
        this.context.clip();
      }
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
      maxX: -(-(this.mapSize.width * this.tileSize.width / 2) + this.canvas.width / 2),
      maxY: +this.canvas.height / 2,
      minY: -(this.mapSize.height * this.tileSize.height) + (this.canvas.height / 2 - 135)
    };
  }

  shouldRender(x, y, width, height)   {
    return x + width  >= -this.origin.x && x <= -this.origin.x + this.canvas.width
        && y + height >= -this.origin.y && y <= -this.origin.y + this.canvas.height;
  }

  clear() {
    this.context.clearRect(-this.origin.x, -this.origin.y, this.canvas.width * 100, this.canvas.height * 100);
  }

  onMouseClick(mouse, mouseX, mouseY) {
    const posX = mouseX - this.canvas.origin.x;
    const posY = mouseY - this.canvas.origin.y;

    for (var x = 0 ; x < this.mapSize.width; ++x) {
      for (var y = 0 ; y < this.mapSize.height; ++y) {
        const pos = this.getPointFor(x, y);
        if (posX >= pos.x && posX <= pos.x + this.tileSize.width &&
            posY >= pos.y && posY <= pos.y + this.tileSize.height)
        {
          this.level.displayConsoleMessage(`Going to [${x}, ${y}]`);
          this.level.tileClicked(x, y);
          return ;
        }
      }
    }
  }
};
