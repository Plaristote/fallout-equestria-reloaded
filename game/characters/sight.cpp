#include "sight.h"
#include "game.h"
#include <cmath>

CharacterSight::CharacterSight(QObject *parent) : ParentType(parent)
{
  fieldOfView = new FieldOfView(reinterpret_cast<Character&>(*this));
}

CharacterSight::~CharacterSight()
{
  delete fieldOfView;
}

bool CharacterSight::hasLineOfSight(const DynamicObject* other) const
{
  return hasSightFrom(other, position);
}

bool CharacterSight::hasSightFrom(const DynamicObject* other, QPoint pos)
{
  auto* level = Game::get()->getLevel();

  if (level)
  {
    auto*  grid   = level->getGrid();
    QPoint target = other->getPosition();
    int    score  = grid->getVisionQuality(pos.x(), pos.y(), target.x(), target.y());

    return score > 0;
  }
  return false;
}

float CharacterSight::getDistance(const DynamicObject* target) const
{
  return getDistance(target->getPosition());
}

float CharacterSight::getDistance(DynamicObject* target) const
{
  return getDistance(target->getPosition());
}

float CharacterSight::getDistance(QPoint other) const
{
  auto self  = getPosition();
  auto a = self.x() - other.x();
  auto b = self.y() - other.y();

  return std::sqrt(static_cast<float>(a * a + b * b));
}
