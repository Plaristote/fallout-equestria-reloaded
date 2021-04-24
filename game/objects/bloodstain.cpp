#include "bloodstain.h"
#include "game/dices.hpp"
#include <QDebug>

BloodStain::BloodStain(QObject *parent) : DynamicObject(parent)
{
  connect(this, &Sprite::animationFinished, this, &BloodStain::onAnimationEnded);
}

void BloodStain::initialize()
{
  int     stainId    = Dices::Throw(3);
  QString spriteName = "blood-stain#" + QString::number(stainId);

  qDebug() << "Bloodztain initialized:" << spriteName;
  setSpriteName(spriteName);
  setAnimation("spill");
  blocksPath = false;
  emit blocksPathChanged();
}

void BloodStain::onAnimationEnded()
{
  setAnimation("idle");
}
