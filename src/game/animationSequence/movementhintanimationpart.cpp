#include "movementhintanimationpart.h"
#include "game.h"

MovementHintAnimationPart::MovementHintAnimationPart(QPoint position)
{
  TileLayer* layer;
  Tile*      tile;

  sprite = new Sprite();
  level  = LevelTask::get();
  layer  = level->getTileMap()->getLayer("ground");
  tile   = layer->getTile(position.x(), position.y());
  sprite->setProperty("floating", false);
  sprite->setSpriteName("misc");
  sprite->setAnimation("movement-hint");
  if (tile)
    from = to = tile->getRenderPosition();
  QObject::connect(sprite, &Sprite::animationFinished, std::bind(&SpriteAnimationPart::onAnimationFinished, this));
}
