#include "worldmap.h"
#include "worldmapcity.h"
#include "game.h"
#include <QDebug>
#include <QSize>
#include <QJsonArray>

WorldMap::WorldMap(QObject* parent) : QObject(parent)
{
  updateTimer.setInterval(50);
  updateTimer.setSingleShot(false);
  connect(&updateTimer, &QTimer::timeout, this, &WorldMap::update);
  connect(this, &WorldMap::targetPositionChanged, this, &WorldMap::onTargetPositionChanged);
  connect(this, &WorldMap::currentPositionChanged, this, &WorldMap::onCurrentPositionChanged);
  timeManager = Game::get()->getTimeManager();
}

QJsonObject WorldMap::save() const
{
  QJsonObject data;
  QJsonArray discoveredJson;
  QJsonArray citiesJson;

  for (auto it = discovered.begin() ; it != discovered.end() ; ++it)
    discoveredJson << *it;
  for (auto it = cities.begin() ; it != cities.end() ; ++it)
  {
    QJsonObject cityJson;

    cityJson.insert("name", (*it)->getName());
    cityJson.insert("x",    (*it)->getPosition().x());
    cityJson.insert("y",    (*it)->getPosition().y());
    cityJson.insert("size", (*it)->getSize());
  }
  data.insert("discovered", discoveredJson);
  data.insert("cities",     citiesJson);
  data.insert("playerX",    currentPosition.x());
  data.insert("playerY",    currentPosition.y());
  return data;
}

void WorldMap::load(const QJsonObject& data)
{
  QJsonArray citiesJson;

  mapSize   = QSize(1000, 2000);
  caseSize  = QSize(100, 100);
  caseCount = QSize(mapSize.width() / caseSize.width(), mapSize.height() / caseSize.height());
  if (data["discovered"].isUndefined())
  {
    for (int i = caseCount.width() * caseCount.height() ; i > 0 ; --i)
      discovered << false;
  }
  else
  {
    QJsonArray discoveredJson = data["discovered"].toArray();

    discovered.reserve(caseCount.width() * caseCount.height());
    for (auto it = discoveredJson.begin() ; it != discoveredJson.end() ; ++it)
      discovered << it->toBool();
  }

  for (auto it = citiesJson.begin() ; it != citiesJson.end() ; ++it)
  {
    QJsonObject cityJson = it->toObject();
    auto* city = new WorldMapCity(this);

    city->setName(cityJson["name"].toString());
    city->setPosition(QPoint(cityJson["x"].toInt(), cityJson["y"].toInt()));
    city->setSize(cityJson["size"].toInt());
    cities << city;
  }

  currentPosition.setX(data["playerX"].toInt(300));
  currentPosition.setY(data["playerY"].toInt(184));
  revealCaseAt(currentPosition);

  // Draftin citiez
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

bool WorldMap::isVisible(int x, int y) const
{
  int offset = y * caseCount.width() + x;

  if (offset < discovered.size())
    return discovered[offset];
  return false;
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
  Game::get()->advanceTime(14);
}

void WorldMap::onCurrentPositionChanged()
{
  revealCaseAt(currentPosition);
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

void WorldMap::revealCaseAt(const QPoint position)
{
  int caseX = position.x() / caseSize.width();
  int caseY = position.y() / caseSize.height();
  int offset = caseY * caseCount.width() + caseX;

  if (offset < discovered.size() && !discovered[offset])
  {
    discovered[offset] = true;
    emit caseRevealed(caseX, caseY);
  }
}
