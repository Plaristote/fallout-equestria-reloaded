#include "orientedsprite.h"

static const QMap<OrientedSprite::Direction, QString> orientationSuffix{
  {OrientedSprite::NoDir,          ""},
  {OrientedSprite::UpperDir,       "up"},
  {OrientedSprite::LeftDir,        "left"},
  {OrientedSprite::RightDir,       "right"},
  {OrientedSprite::BottomDir,      "down"},
  {OrientedSprite::UpperLeftDir,   "left"},
  {OrientedSprite::UpperRightDir,  "right"},
  {OrientedSprite::BottomLeftDir,  "left"},
  {OrientedSprite::BottomRightDir, "right"}
};

const QMap<QString, OrientedSprite::Direction> OrientedSprite::directionByName{
  {"up",           OrientedSprite::UpperDir},
  {"up-left",      OrientedSprite::UpperLeftDir},
  {"up-right",     OrientedSprite::UpperRightDir},
  {"left",         OrientedSprite::LeftDir},
  {"right",        OrientedSprite::RightDir},
  {"down",         OrientedSprite::BottomDir},
  {"bottom",       OrientedSprite::BottomDir},
  {"bottom-left",  OrientedSprite::BottomLeftDir},
  {"bottom-right", OrientedSprite::BottomRightDir}
};

OrientedSprite::OrientedSprite(QObject *parent) : ParentType(parent)
{
}

QString OrientedSprite::getAnimationBaseName() const
{
  static const QRegularExpression regexp("-(up|left|down|right)$");

  return getAnimation().replace(regexp, "");
}

void OrientedSprite::setAnimation(const QString &animationName)
{
  if (orientation != NoDir)
  {
    QString completeAnimationName = animationName + '-' + orientationSuffix[orientation];

    Sprite::setAnimation(completeAnimationName);
    if (getAnimationBaseName() != animationName && animationName != "idle") // fallback
      ParentType::setAnimation("idle");
  }
  else
    Sprite::setAnimation(animationName);
}

void OrientedSprite::setOrientation(const QString& name)
{
  auto it = directionByName.find(name);

  if (it != directionByName.end())
    setOrientation(directionByName[name]);
  else
    qDebug() << "Unknown direction" << name << "; accepted values are:" << directionByName.keys();
}

void OrientedSprite::setOrientation(Direction newOrientation)
{
  if (orientation != newOrientation)
  {
    orientation = newOrientation;
    setAnimation(getAnimationBaseName());
    emit orientationChanged();
  }
}

QString OrientedSprite::getOrientationName() const
{
  return orientationSuffix[orientation];
}

void OrientedSprite::load(const QJsonObject& data)
{
  orientation = static_cast<Direction>(data[">"].toInt(NoDir));
  ParentType::load(data);
}

void OrientedSprite::save(QJsonObject& data) const
{
  if (orientation != NoDir)
    data[">"] = static_cast<unsigned short>(orientation);
  ParentType::save(data);
}
