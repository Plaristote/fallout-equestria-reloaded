function drawImage(context, options) {
  try {
    context.drawImage(
      options.source,
      options.clippedRect.x,      options.clippedRect.y,
      options.clippedRect.width,  options.clippedRect.height,
      options.offset.x,           options.offset.y,
      options.renderedSize.width, options.renderedSize.height
    );
  } catch (err) {
    console.log("CanvasRenderer::renderImage: failed to render", options.source, options.clippedRect, ": ", err)
  }
}

export class CanvasRenderer {
  constructor(canvas) {
    this.canvas = canvas;
    this.context = canvas.getContext("2d");
  }

  get origin() {
    return this.canvas.origin;
  }

  shouldRender(x, y, width, height) {
    return x + width  >= -this.origin.x && x <= -this.origin.x + this.canvas.width
        && y + height >= -this.origin.y && y <= -this.origin.y + this.canvas.height;
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
      drawImage(this.context, {
        source:        source,
        clippedRect:   clippedRect,
        offset:        offset,
        renderedSize:  { width: width, height: height }
      });
      return true;
    }
    return false;
  }

  clear() {
    this.context.clearRect(-this.origin.x, -this.origin.y, this.canvas.width * 10, this.canvas.height * 10);
  }
}
