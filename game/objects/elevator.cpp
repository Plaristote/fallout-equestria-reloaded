#include "elevator.h"
#include "game.h"

Elevator::Elevator(QObject *parent) : DynamicObject(parent)
{
  floorA = floorB = NULL_FLOOR;
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

static LevelGrid::CaseContent* getGridCase(QPoint position, unsigned char floor)
{
  LevelGrid* grid = Game::get()->getLevel()->getFloorGrid(floor);

  return grid ? grid->getGridCase(position) : nullptr;
}

void Elevator::disconnectCases()
{
  if (isValid())
  {
    auto* gridCaseA = getGridCase(positionA, floorA);
    auto* gridCaseB = getGridCase(positionB, floorB);

    if (gridCaseA && gridCaseB)
      gridCaseA->disconnectFrom(gridCaseB);
  }
}

void Elevator::connectCases()
{
  if (isValid())
  {
    auto* gridCaseA = getGridCase(positionA, floorA);
    auto* gridCaseB = getGridCase(positionB, floorB);

    if (gridCaseA && gridCaseB)
      gridCaseA->connectWith(gridCaseB);
  }
}

void Elevator::onLevelFloorChanged()
{
  auto activeFloor = static_cast<unsigned char>(Game::get()->getLevel()->getCurrentFloor());

  if (activeFloor == floorA || activeFloor == floorB)
  {
    setCurrentFloor(activeFloor);
    setAnimation(activeFloor == floorA ? "A" : "B");
  }
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
