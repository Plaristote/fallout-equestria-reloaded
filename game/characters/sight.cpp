#include "sight.h"
#include "game.h"
#include <cmath>

CharacterSight::CharacterSight(QObject *parent) : ParentType(parent)
{
  fieldOfView = new FieldOfView(reinterpret_cast<Character&>(*this));
  connect(this, &CharacterDiplomacy::diplomacyUpdated, this, &CharacterSight::refreshFieldOfView);
  connect(fieldOfView, &FieldOfView::characterDetected, this, &CharacterSight::onCharacterDetected, Qt::QueuedConnection);
  connect(fieldOfView, &FieldOfView::refreshed, this, &CharacterSight::onRefreshed);
}

CharacterSight::~CharacterSight()
{
  delete fieldOfView;
}

bool CharacterSight::hasLineOfSight(const DynamicObject* other) const
{
  return hasSightFrom(other, getPoint());
}

bool CharacterSight::hasSightFrom(const DynamicObject* other, Point pos)
{
  if (other)
    return hasSightFrom(other->getPoint(), pos);
  return false;
}

bool CharacterSight::hasLineOfSight(Point target) const
{
  return hasSightFrom(target, getPoint());
}

bool CharacterSight::hasSightFrom(Point target, Point pos)
{
  auto* level = Game::get()->getLevel();

  if (target.z == pos.z && level)
  {
    auto*  grid   = level->getFloorGrid(target.z);
    int    score  = grid->getVisionQuality(pos.x, pos.y, target.x, target.y);

    return score > 0;
  }
  return false;
}

float CharacterSight::getDistance(const DynamicObject* target) const
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

void CharacterSight::refreshFieldOfView()
{
  if (fieldOfView && Game::get()->getLevel())
  {
    fieldOfView->reset();
    fieldOfView->runTask();
  }
}

void CharacterSight::onRefreshed()
{
  if (script && script->hasMethod("onObservationTriggered"))
    script->call("onObservationTriggered");
}

void CharacterSight::onCharacterDetected(Character* character)
{
  if (script && script->hasMethod("onCharacterDetected"))
  {
    QJSValueList args;

    args << character->asJSValue();
    script->call("onCharacterDetected", args);
  }
}
