export const RenderContextMode = {
  Normal:           0,
  ClipAroundPlayer: 1
};

function startClipAroundPlayer() {
  const sprite = this.level.player;

  if (sprite) {
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

export class RenderContextManager {
  constructor(parent) {
    this.parent         = parent;
    this.context        = parent.context;
    this.mode           = RenderContextMode.Normal;
    this.playerRendered = false;
  }

  finalize() { this.requireMode(RenderContextMode.Normal); }

  requireMode(mode) {
    if (mode !== this.mode) {
      this.context.restore();
      switch (mode) {
      case RenderContextMode.Normal:
        break ;
      case RenderContextMode.ClipAroundPlayer:
        if (!this.playerRendered) { return; }
        startClipAroundPlayer.bind(this.parent)();
        break ;
      }
      this.mode = mode;
    }
  }
}
