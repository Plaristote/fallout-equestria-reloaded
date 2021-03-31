#include "detectable.h"
#include "game/character.h"
#include "game/characters/actionqueue.h"
#include "game/dices.hpp"

DetectableComponent::DetectableComponent(QObject *parent) : ParentType(parent)
{
  connect(this, &DetectableComponent::detected, this, &DetectableComponent::hiddenChanged);
  connect(this, &DetectableComponent::detected, this, &DetectableComponent::onDetected);
}

void DetectableComponent::load(const QJsonObject& data)
{
  hidden   = data["hidden"].toBool(false);
  sneaking = data["sneaking"].toBool(false);
  interruptOnDetection = data["interruptOnDetection"].toBool(false);
  ParentType::load(data);
}

void DetectableComponent::save(QJsonObject& data) const
{
  if (hidden)   { data["hidden"]   = hidden; }
  if (sneaking) { data["sneaking"] = sneaking; }
  if (interruptOnDetection) { data["interruptOnDetection"] = interruptOnDetection; }
  ParentType::save(data);
}

void DetectableComponent::toggleSneaking(bool value)
{
  if (sneaking != value)
  {
    hidden = sneaking = value;
    emit hiddenChanged();
    emit sneakingChanged();
  }
}

bool DetectableComponent::tryDetection(Character* character)
{
  unsigned int perception = character->getStatistics()->property("perception").toUInt();
  float        distance   = character->getDistance(reinterpret_cast<DynamicObject*>(this));
  int          result     = Dices::Throw(perception);

  qDebug() << "Try detection on distance" << distance << "with throw" << result;
  if (result >= static_cast<int>(distance))
  {
    if (interruptOnDetection)
      character->getActionQueue()->reset();
    hidden = false;
    emit detected();
    return true;
  }
  return false;
}

void DetectableComponent::onDetected()
{
  if (script && script->hasMethod("onDetected"))
    script->call("onDetected");
}
