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

int DetectableComponent::getSneakAbility() const
{
  if (script && script->getObject().hasProperty("sneak"))
    return script->getObject().property("sneak").toInt();
  return 50;
}

bool DetectableComponent::tryDetection(const Character* character)
{
  unsigned int perception = character->getStatistics()->property("perception").toUInt();
  float        distance   = character->getDistance(reinterpret_cast<DynamicObject*>(this));
  int          difficulty = getSneakAbility() + static_cast<int>(distance * 5);
  int          result     = Dices::Throw(perception * 11 + 60 + (perception > 6 ? 15 : 0));

  qDebug() << "(i) Detection attempt on" << this->property("objectName").toString() << "by" << character->getObjectName()
           << ": threshold is" << getSneakAbility() << '+' << distance << '*' << 5
           << ", roll is" << perception << '*' << 12 << '+' << (perception > 6 ? 75 : 60)
           << " -> " << result << ">=" << difficulty;
  if (result >= difficulty)
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
