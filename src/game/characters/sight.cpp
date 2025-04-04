#include "sight.h"
#include "game/leveltask.h"
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
  auto* level = LevelTask::get();

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

  // use pathvinding distance
  return static_cast<float>(std::max(std::abs(a), std::abs(b)));
  // or use actual distance
  //return std::sqrt(static_cast<float>(a * a + b * b));
}

void CharacterSight::refreshFieldOfView()
{
  if (fieldOfView && LevelTask::get())
  {
    fieldOfView->reset();
    fieldOfView->runTask();
  }
}

void CharacterSight::onRefreshed()
{
  scriptCall("onObservationTriggered");
}

void CharacterSight::onCharacterDetected(Character* character)
{
  if (fieldOfView->isDetected(character))
    scriptCall("onCharacterDetected", QJSValueList() << character->asJSValue());
}
