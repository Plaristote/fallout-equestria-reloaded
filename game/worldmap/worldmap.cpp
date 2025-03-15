#include "worldmap.h"
#include "worldmapcity.h"
#include "game.h"
#include <QDebug>
#include <QSize>
#include <QJsonArray>
#include <cmath>

WorldMap::WorldMap(QObject* parent) : QObject(parent)
{
  updateTimer.setInterval(50);
  updateTimer.setSingleShot(false);
  connect(&updateTimer, &QTimer::timeout, this, &WorldMap::update);
  connect(this, &WorldMap::mapSizeChanged, this, &WorldMap::onMapSizeChanged);
  connect(this, &WorldMap::targetPositionChanged, this, &WorldMap::onTargetPositionChanged);
  connect(this, &WorldMap::currentPositionChanged, this, &WorldMap::onCurrentPositionChanged);
  connect(Game::get(), &Game::encounterTriggered, this, &WorldMap::onEncounterTriggered);
  connect(Game::get(), &Game::encounterNotify,    this, &WorldMap::onEncounterTriggered);
  connect(Game::get(), &Game::encounterTriggered, this, &WorldMap::encounterTriggered);
  connect(Game::get(), &Game::encounterNotify,    this, &WorldMap::encounterNotify);
  timeManager = Game::get()->getTimeManager();
  terrainData = QImage(ASSETS_PATH + "worldmap-terrain.png");
}

bool WorldMap::canBeMovedOn(QPoint coordinates) const
{
  if (!terrainData.isNull())
  {
    float ratioY = static_cast<float>(terrainData.height()) / mapSize.height();
    float ratioX = static_cast<float>(terrainData.width())  / mapSize.width();
    QRgb pixel = terrainData.pixel(coordinates.x() * ratioX, coordinates.y() * ratioY);

    return qAlpha(pixel) == 0;
  }
  return true;
}

QJsonObject WorldMap::save() const
{
  QJsonObject data;
  QJsonArray discoveredJson;
  QJsonArray citiesJson;
  QJsonArray zonesJson;

  for (auto it = discovered.begin() ; it != discovered.end() ; ++it)
    discoveredJson << *it;
  for (auto it = cities.begin() ; it != cities.end() ; ++it)
  {
    QJsonObject cityJson;

    (*it)->save(cityJson);
    citiesJson << cityJson;
  }
  for (auto it = zones.begin() ; it != zones.end() ; ++it)
    zonesJson << (*it)->save();
  if (!Game::get()->property("isGameEditor").toBool())
    data.insert("discovered", discoveredJson);
  data.insert("cities",     citiesJson);
  data.insert("zones",      zonesJson);
  data.insert("playerX",    currentPosition.x());
  data.insert("playerY",    currentPosition.y());
  data.insert("width",      mapSize.width());
  data.insert("height",     mapSize.height());
  data.insert("caseWidth",  caseSize.width());
  data.insert("caseHeight", caseSize.height());
  data.insert("visible",    QJsonValue::fromVariant(QVariant::fromValue(discoveredCities)));
  qDebug() << "Saving worldmap" << QJsonDocument(data).toJson();
  return data;
}

void WorldMap::onMapSizeChanged()
{
  caseCount = QSize(
    mapSize.width()  / std::max(caseSize.width(), 1),
    mapSize.height() / std::max(caseSize.height(), 1)
  );
  emit caseCountChanged();
}

void WorldMap::load(const QJsonObject& data)
{
  QJsonArray citiesJson = data["cities"].toArray();
  QJsonArray zonesJson = data["zones"].toArray();

  discoveredCities = data["visible"].toVariant().toStringList();
  mapSize  = QSize(data["width"].toInt(),     data["height"].toInt());
  caseSize = QSize(data["caseWidth"].toInt(), data["caseHeight"].toInt());
  caseCount = QSize(
    mapSize.width()  / std::max(caseSize.width(), 1),
    mapSize.height() / std::max(caseSize.height(), 1)
  );
  if (!Game::get()->property("isGameEditor").toBool())
  {
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
  }

  for (auto it = citiesJson.begin() ; it != citiesJson.end() ; ++it)
  {
    QJsonObject cityJson = it->toObject();
    auto* city = new WorldMapCity(this);

    city->load(cityJson);
    cities << city;
  }

  for (auto it = zonesJson.begin() ; it != zonesJson.end() ; ++it)
  {
    auto* zone = new WorldMapZone(this);

    zone->load(it->toObject());
    zones << zone;
  }

  currentPosition.setX(data["playerX"].toInt(0));
  currentPosition.setY(data["playerY"].toInt(0));
  targetPosition = currentPosition.toPoint();

  emit mapSizeChanged();
  emit citiesChanged();
  emit zonesChanged();
  emit discoveredCitiesChanged();
}

static float axisMovement(float current, int final, float speed)
{
  float result = final;

  if      (current < final && final - current > speed) { result = current + speed; }
  else if (current > final && current - final > speed) { result = current - speed; }
  return std::abs(result - final) < speed ? final : result;
}

bool WorldMap::isVisible(int x, int y) const
{
  int offset = y * caseCount.width() + x;

  if (offset < discovered.size())
    return discovered[offset];
  return true;
}

float WorldMap::getCurrentMovementSpeed() const
{
  const auto zones = getCurrentZoneList();
  float movementSpeed = 5.f;

  for (const WorldMapZone* zone : zones)
  {
    auto zoneSpeed = static_cast<float>(zone->getMovementSpeed());

    if (zoneSpeed < movementSpeed)
      movementSpeed = zoneSpeed;
  }
  return movementSpeed;
}

void WorldMap::update()
{
  if (!paused)
  {
    float   movementSpeed = getCurrentMovementSpeed();
    float   movementSpeedX = movementSpeed;
    float   movementSpeedY = movementSpeed;
    float   distX = std::max<float>(currentPosition.x(), targetPosition.x()) - std::min<float>(currentPosition.x(), targetPosition.x());
    float   distY = std::max<float>(currentPosition.y(), targetPosition.y()) - std::min<float>(currentPosition.y(), targetPosition.y());
    QPointF nextPosition;

    float distance = std::sqrt(distX * distX + distY * distY);
    float ratio = movementSpeed / distance;
    movementSpeedX = distX * ratio;
    movementSpeedY = distY * ratio;

    nextPosition.setX(axisMovement(currentPosition.x(), targetPosition.x(), movementSpeedX));
    nextPosition.setY(axisMovement(currentPosition.y(), targetPosition.y(), movementSpeedY));
    if (canBeMovedOn(nextPosition.toPoint()))
    {
      currentPosition = nextPosition;
      emit currentPositionChanged();
    }
    else
      targetPosition = currentPosition.toPoint();
    if (currentPosition.toPoint() == targetPosition)
      onTargetPositionReached();
    Game::get()->advanceTime(14);
  }
}

void WorldMap::onCurrentPositionChanged()
{
  revealCaseAt(currentPosition.toPoint());
}

void WorldMap::onTargetPositionChanged()
{
  qDebug() << "Starting update timer";
  updateTimer.start();
  emit movingChanged();
}

void WorldMap::onTargetPositionReached()
{
  qDebug() << "Stopping update timer";
  updateTimer.stop();
  emit movingChanged();
}

void WorldMap::restart()
{
  if (paused)
  {
    paused = false;
    emit pausedChanged();
  }
  if (currentPosition != targetPosition)
    onTargetPositionChanged();
}

WorldMapCity* WorldMap::getCity(const QString &name) const
{
  for (WorldMapCity* city : cities)
  {
    if (city->getName() == name)
      return city;
  }
  return nullptr;
}

WorldMapZone* WorldMap::getZone(const QString &name) const
{
  for (WorldMapZone* zone : zones)
  {
    if (zone->property("name").toString() == name)
      return zone;
  }
  return nullptr;
}

void WorldMap::moveToCity(WorldMapCity* city)
{
  currentPosition = targetPosition = city->getPosition();
  emit currentPositionChanged();
}

void WorldMap::getIntoCity(WorldMapCity* city)
{
  QString levelSource = city->getLevel();
  bool visited = Game::get()->getDataEngine()->hasLevelBeenVisited(city->getLevel());

  if (discoveredCities.indexOf(city->getName()) < 0)
  {
    discoveredCities << city->getName();
    emit discoveredCitiesChanged();
  }
  else if (city->hasSplashscreen() > 0 && visited)
  {
    emit splashscreenEntered(city);
    return ;
  }
  emit cityEntered(levelSource);
}

void WorldMap::getIntoWasteland(QPoint poz)
{
  qDebug() << "Get into wazteland" << poz;
}

void WorldMap::revealCaseAt(const QPoint position)
{
  int caseX = position.x() / std::max(caseSize.width(), 1);
  int caseY = position.y() / std::max(caseSize.height(), 1);
  unsigned int offset = static_cast<unsigned int>(caseY * caseCount.width() + caseX);

  if (offset < discovered.size() && !discovered[offset])
  {
    discovered[offset] = true;
    emit caseRevealed(caseX, caseY);
  }
}

void WorldMap::revealCity(const QString &name)
{
  revealCity(getCity(name));
}

void WorldMap::revealCity(WorldMapCity* city)
{
  if (city)
  {
    auto index = discoveredCities.indexOf(city->getName());

    if (index < 0)
    {
      discoveredCities << city->getName();
      revealCaseAt(city->getPosition());
      emit discoveredCitiesChanged();
    }
  }
  else
    qDebug() << "WorldMap::revealCity: parameter is null";
}

void WorldMap::setPosition(int x, int y)
{
  targetPosition = QPoint(x, y);
  currentPosition = targetPosition;
  emit currentPositionChanged();
}

WorldMapCity* WorldMap::createCity(const QString &name)
{
  WorldMapCity* city = new WorldMapCity(this);

  city->setName(name);
  city->setSize(caseSize.width());
  cities << city;
  emit citiesChanged();
  return city;
}

WorldMapZone* WorldMap::createZone(const QString &name)
{
  WorldMapZone* zone = new WorldMapZone(this);

  zone->setProperty("name", name);
  zones << zone;
  emit zonesChanged();
  return zone;
}

void WorldMap::removeCity(WorldMapCity* city)
{
  auto index = cities.indexOf(city);

  if (index >= 0)
  {
    cities.removeAt(index);
    emit citiesChanged();
  }
  city->deleteLater();
}

void WorldMap::removeZone(WorldMapZone* zone)
{
  auto index = zones.indexOf(zone);

  if (index >= 0)
  {
    zones.removeAt(index);
    emit zonesChanged();
  }
  zone->deleteLater();
}

QPoint WorldMap::getCaseAt(QPoint position) const
{
  return QPoint(
    position.x() / caseSize.width(),
    position.y() / caseSize.height()
  );
}

WorldMapZone* WorldMap::getCurrentZone() const
{
  QPoint currentCase = getCaseAt(currentPosition.toPoint());

  for (WorldMapZone* zone : std::as_const(zones))
  {
    if (zone->containsCase(currentCase.x(), currentCase.y()))
      return zone;
  }
  return nullptr;
}

WorldMapCity* WorldMap::getCurrentCity() const
{
  for (WorldMapCity* city : cities)
  {
    if (city->isInside(currentPosition.toPoint()))
      return city;
  }
  return nullptr;
}

QVector<WorldMapZone*> WorldMap::getCurrentZoneList() const
{
  QVector<WorldMapZone*> list;
  QPoint currentCase = getCaseAt(currentPosition.toPoint());

  for (WorldMapZone* zone : std::as_const(zones))
  {
    if (zone->containsCase(currentCase.x(), currentCase.y()))
      list.push_back(zone);
  }
  return list;
}

QStringList WorldMap::getCurrentZones() const
{
  const auto  zones = getCurrentZoneList();
  QStringList list;

  list.reserve(zones.size());
  for (WorldMapZone* zone : zones)
    list.push_back(zone->getName());
  return list;
}

void WorldMap::onEncounterTriggered()
{
  updateTimer.stop();
}
