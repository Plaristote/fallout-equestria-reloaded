#include "movementhintanimationpart.h"
#include "game.h"

MovementHintAnimationPart::MovementHintAnimationPart(QPoint position)
{
  TileLayer* layer;
  Tile*      tile;

  sprite = new Sprite(this);
  level  = Game::get()->getLevel();
  layer  = level->getTileMap()->getLayer("ground");
  tile   = layer->getTile(position.x(), position.y());
  sprite->setProperty("floating", false);
  sprite->setSpriteName("misc");
  sprite->setAnimation("movement-hint");
  if (tile)
    from = to = tile->getRenderPosition();
  connect(sprite, &Sprite::animationFinished, this, &SpriteAnimationPart::onAnimationFinished);
}
