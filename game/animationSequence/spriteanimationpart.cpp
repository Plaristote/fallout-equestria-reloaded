#include "spriteanimationpart.h"
#include <QJSValue>
#include "game.h"
#include "game/leveltask.h"
#include "game/sprite.h"

SpriteAnimationPart::~SpriteAnimationPart()
{
  level->unregisterVisualEffect(sprite);
}

void SpriteAnimationPart::initialize(QJSValue &value)
{
  sprite = new Sprite(this);
  level  = Game::get()->getLevel();
  sprite->setProperty("floating", true);
  sprite->setSpriteName(value.property("name").toString());
  sprite->setAnimation(value.property("animation").toString());
  from = QPoint(
    value.property("fromX").toInt(),
    value.property("fromY").toInt()
  );
  to = QPoint(
    value.property("toX").toInt(),
    value.property("toY").toInt()
  );
  sprite->setMovementSpeed(static_cast<float>(value.property("speed").toNumber()));
  connect(sprite, &Sprite::movementFinished, this, &SpriteAnimationPart::onAnimationFinished);
}

void SpriteAnimationPart::start()
{
  level->registerVisualEffect(sprite);
  sprite->setRenderPosition(from);
  sprite->moveToCoordinates(to);
}
