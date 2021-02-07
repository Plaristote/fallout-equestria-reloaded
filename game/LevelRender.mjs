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
    this.eachCase(this.renderCoordinates.bind(this));
  }

  eachCase(callback) {
    for (var x = 0 ; x < this.mapSize.width; ++x) {
      for (var y = 0 ; y < this.mapSize.height; ++y) {
        callback(x, y);
      }
    }
  }

  renderTile(x, y) {
    const tile = this.layers.ground.getTile(x, y);

    if (tile)
      this.renderImage("../" + tile.image, tile.renderPosition, this.tileSize.width, this.tileSize.height, tile.clippedRect);
  }

  renderCoordinates(x, y) {
    const wall = this.layers.walls.getTile(x, y);
    const dynamicObject = this.level.getOccupantAt(x, y);

    if (wall)
      this.renderWall(wall, x, y);
    else if (dynamicObject)
      this.renderSprite(dynamicObject);
  }

  renderWall(tile, x, y) {
    this.renderImage("../" + tile.image, this.getPointFor(x, y), this.wallSize.width, this.wallSize.height, tile.clippedRect);
  }

  renderSprite(sprite) {
    const offset      = sprite.getSpritePosition();
    const clippedRect = sprite.getClippedRect();
    const extraHeight = clippedRect.height - this.tileSize.height;

    offset.y -= extraHeight;
    offset.x += this.tileSize.width / 2 - clippedRect.width / 2;
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
