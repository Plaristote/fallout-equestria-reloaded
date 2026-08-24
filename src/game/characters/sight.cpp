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
    auto* grid  = level->getFloorGrid(target.z);
    int   score = grid ? grid->getVisionQuality(pos.x, pos.y, target.x, target.y) : 0;

    return score > 0;
  }
  return false;
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
