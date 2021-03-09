#include "worldmap.h"
#include "game.h"
#include <QDebug>

WorldMap::WorldMap(QObject* parent) : QObject(parent)
{
  updateTimer.setInterval(50);
  updateTimer.setSingleShot(false);
  connect(&updateTimer, &QTimer::timeout, this, &WorldMap::update);
  connect(this, &WorldMap::targetPositionChanged, this, &WorldMap::onTargetPositionChanged);
  timeManager = Game::get()->getTimeManager();

  // Draftin citiez
  currentPosition = QPoint(300, 184);
  {
    auto* a = new WorldMapCity(this);
    a->setName("eltest5");
    a->setPosition(QPoint(300, 184));
    a->setSize(80);
    cities << a;
  }

  {
    auto* a = new WorldMapCity(this);
    a->setName("eltest");
    a->setPosition(QPoint(628, 794));
    a->setSize(100);
    cities << a;
  }
}

static int axisMovement(int current, int final, int speed)
{
  int result = final;

  if      (current < final && final - current > speed) { result = current + speed; }
  else if (current > final && current - final > speed) { result = current - speed; }
  return std::abs(result - final) < speed ? final : result;
}

void WorldMap::update()
{
  qDebug() << "WorldMap::update";
  float movementSpeed = 5;
  float movementSpeedX = 5;
  float movementSpeedY = 5;
  int   distX = std::max(currentPosition.x(), targetPosition.x()) - std::min(currentPosition.x(), targetPosition.x());
  int   distY = std::max(currentPosition.y(),  targetPosition.y()) - std::min(currentPosition.y(), targetPosition.y());

  if      (distX > distY) { movementSpeedY = movementSpeed * (static_cast<float>(distY) / static_cast<float>(distX)); }
  else if (distY > distX) { movementSpeedX = movementSpeed * (static_cast<float>(distX) / static_cast<float>(distY)); }
  currentPosition.setX(axisMovement(currentPosition.x(), targetPosition.x(), static_cast<int>(movementSpeedX)));
  currentPosition.setY(axisMovement(currentPosition.y(), targetPosition.y(), static_cast<int>(movementSpeedY)));
  emit currentPositionChanged();
  if (currentPosition == targetPosition)
    emit onTargetPositionReached();
  timeManager->addElapsedTime(DateTime::Minutes(14));
}

void WorldMap::onTargetPositionChanged()
{
  qDebug() << "Starting update timer";
  updateTimer.start();
}

void WorldMap::onTargetPositionReached()
{
  qDebug() << "Stopping update timer";
  updateTimer.stop();
}

void WorldMap::getIntoCity(WorldMapCity* city)
{
  qDebug() << "Rekuezted get into city" << city->getName();
  emit cityEntered(city->getName());
}

void WorldMap::getIntoWasteland(QPoint poz)
{
  qDebug() << "Get into wazteland" << poz;
}
