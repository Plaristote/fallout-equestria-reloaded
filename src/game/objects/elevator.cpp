#include "elevator.h"
#include "game/leveltask.h"

Elevator::Elevator(QObject *parent) : DynamicObject(parent)
{
  floorA = floorB = NULL_FLOOR;
}

QStringList Elevator::getAvailableInteractions()
{
  if (script && script->hasMethod("getAvailableInteractions"))
    return script->call("getAvailableInteractions").toVariant().toStringList();
  return QStringList() << "use" << "look";
}

void Elevator::setPositionA(QPoint position_, unsigned char floor_)
{
  disconnectCases();
  positionA = position_;
  floorA    = floor_;
  connectCases();
  emit elevatorChanged();
}

void Elevator::setPositionB(QPoint position_, unsigned char floor_)
{
  disconnectCases();
  positionB = position_;
  floorB    = floor_;
  connectCases();
  emit elevatorChanged();
}

void Elevator::disconnectCases()
{
  if (isValid())
  {
    auto& pathfinder = LevelTask::get()->getPathfinder();

    pathfinder.disconnectCases(
      {positionA.x(), positionA.y(), floorA},
      {positionB.x(), positionB.y(), floorB}
    );
  }
}

void Elevator::connectCases()
{
  if (isValid())
  {
    auto& pathfinder = LevelTask::get()->getPathfinder();

    pathfinder.connectCases(
      {positionA.x(), positionA.y(), floorA},
      {positionB.x(), positionB.y(), floorB}
    );
  }
}

void Elevator::onLevelFloorChanged()
{
  auto activeFloor = static_cast<unsigned char>(LevelTask::get()->getCurrentFloor());

  if (activeFloor == floorA || activeFloor == floorB)
  {
    setCurrentFloor(activeFloor);
    setAnimation(activeFloor == floorA ? "A" : "B");
  }
}

bool Elevator::triggerInteraction(Character* character, const QString& interactionType)
{
  bool handled = DynamicObject::triggerInteraction(character, interactionType);

  if (!handled && interactionType == "use")
  {
    LevelTask* level = LevelTask::get();

    if (character->getCurrentFloor() == floorA)
      level->setCharacterPosition(character, positionB.x(), positionB.y(), floorB);
    else
      level->setCharacterPosition(character, positionA.x(), positionA.y(), floorA);
    return true;
  }
  return handled;
}

void Elevator::load(const QJsonObject& data)
{
  DynamicObject::load(data);
  positionA.setX(data["ax"].toInt());
  positionA.setY(data["ay"].toInt());
  floorA = static_cast<unsigned char>(data["az"].toInt());
  positionB.setX(data["bx"].toInt());
  positionB.setY(data["by"].toInt());
  floorB = static_cast<unsigned char>(data["bz"].toInt());
  onLevelFloorChanged();
}

void Elevator::save(QJsonObject& data) const
{
  DynamicObject::save(data);
  data["ax"] = positionA.x();
  data["ay"] = positionA.y();
  data["az"] = static_cast<int>(floorA);
  data["bx"] = positionB.x();
  data["by"] = positionB.y();
  data["bz"] = static_cast<int>(floorB);
}
