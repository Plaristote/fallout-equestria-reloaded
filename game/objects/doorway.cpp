#include "doorway.h"
#include "tilemap/tilezone.h"
#include "game.h"
#include "i18n.h"
#include <QJsonArray>

Doorway::Doorway(QObject* parent) : DynamicObject(parent)
{
  lockpickLevel = 1;
  connect(this, &Doorway::openedChanged, this, &Doorway::updateAccessPath);
  connect(this, &Doorway::openedChanged, this, &Doorway::updateAnimation);
  connect(this, &DynamicObject::positionChanged,     this, &Doorway::updateTileConnections);
  connect(this, &OrientedSprite::orientationChanged, this, &Doorway::updateTileConnections);
  connect(this, &OrientedSprite::orientationChanged, this, &Doorway::updateAnimation);
  openSound = closeSound = "door-open";
  lockedSound = "door-locked";
  tileConnections.reserve(8);
}

QVector<Point> Doorway::getInteractionPositions() const
{
  Point          point = getPoint();
  Point          altPoint(point);
  QVector<Point> results{point};

  switch (getOrientation())
  {
  case BottomDir:
    altPoint.y++;
    break ;
  case UpperDir:
    altPoint.y--;
    break ;
  case RightDir:
    altPoint.x++;
    break ;
  case LeftDir:
    altPoint.x--;
    break ;
  default:
    return results;
  }
  results.push_back(altPoint);
  return results;
}

int Doorway::getCoverValue() const
{
  int base = DynamicObject::getCoverValue();

  return opened ? base / 2 : base;
}

void Doorway::removeTileConnections()
{
  for (void* caseContent : std::as_const(tileConnections))
    reinterpret_cast<LevelGrid::CaseConnection*>(caseContent)->doorway = nullptr;
  tileConnections.clear();
}

static const QVector<QPoint> getTileConnectionTargets(OrientedSprite::Direction orientation)
{
  static const QVector<QPoint> upperTargets {QPoint(-1, -1), QPoint(0, -1), QPoint(1, -1)};
  static const QVector<QPoint> bottomTargets{QPoint(-1,  1), QPoint(0,  1), QPoint(1,  1)};
  static const QVector<QPoint> leftTargets  {QPoint(-1, -1), QPoint(-1, 0), QPoint(-1, 1)};
  static const QVector<QPoint> rightTargets {QPoint(1,  -1), QPoint(1,  0), QPoint(1,  1)};

  switch (orientation)
  {
  case OrientedSprite::UpperDir:  return upperTargets;
  case OrientedSprite::BottomDir: return bottomTargets;
  case OrientedSprite::LeftDir:   return leftTargets;
  case OrientedSprite::RightDir:  return rightTargets;
  default:
    qDebug() << "Unsupported Doorway direction";
  }
  return {};
}

void Doorway::updateTileConnections()
{
  QPoint                       origin = getPosition();
  LevelGrid*                   grid = Game::get()->getLevel()->getFloorGrid(getCurrentFloor());
  LevelGrid::CaseContent*      doorwayCase = grid ? grid->getGridCase(origin) : nullptr;

  removeTileConnections();
  for (const QPoint& target : getTileConnectionTargets(getOrientation()))
  {
    LevelGrid::CaseContent*    targetCase = grid->getGridCase(origin + target);
    LevelGrid::CaseConnection* connection = targetCase ? targetCase->connectionWith(doorwayCase) : nullptr;

    if (connection)
    {
      connection->doorway = this;
      tileConnections.push_back(connection);
    }
  }
}

void Doorway::updateAccessPath()
{
  if (hasControlZone())
    toggleZoneBlocked(!opened);
  else
  {
    LevelGrid*              grid = Game::get()->getLevel()->getFloorGrid(getCurrentFloor());
    LevelGrid::CaseContent* doorwayCase = grid ? grid->getGridCase(getPosition()) : nullptr;

    if (doorwayCase)
      doorwayCase->cover = static_cast<char>(getCoverValue());
  }
}

void Doorway::updateAnimation()
{
  if (!destroyed)
    setAnimation(opened ? "open" : "close");
  else
    setAnimation("destroyed");
}

bool Doorway::bustOpen(int damage)
{
  bool success = true;

  if (script && script->hasMethod("onBustOpen"))
    success = script->call("onBustOpen", QJSValueList() << damage).toBool();
  if (success)
  {
    destroyed = true;
    opened = true;
    locked = false;
    emit destroyedChanged();
    emit openedChanged();
    emit lockedChanged();
  }
  return success;
}

bool Doorway::onGoThrough(Character* character)
{
  if (!opened && !destroyed && canGoThrough(character))
    onUse(character);
  return opened || destroyed;
}

bool Doorway::canGoThrough(Character* character) const
{
  if (character == Game::get()->getPlayer())
    return opened || !locked;
  if (!opened && !destroyed)
  {
    QJSValue result = scriptCall("canGoThrough", QJSValueList() << character->asJSValue());

    if (result.isBool())
      return result.toBool();
    return !locked || character->getInventory()->count(keyName) > 0;
  }
  return true;
}

QStringList Doorway::getAvailableInteractions()
{
  if (script && script->hasMethod("getAvailableInteractions"))
    return script->call("getAvailableInteractions").toVariant().toStringList();
  return QStringList() << "use" << "look" << "use-object" << "use-skill";
}

bool Doorway::triggerInteraction(Character *character, const QString &interactionType)
{
  if (interactionType == "use")
    return onUse(character);
  return DynamicObject::triggerInteraction(character, interactionType);
}

bool Doorway::onUse(Character* character)
{
  QJSValue result = scriptCall("onUse", QJSValueList() << character->asJSValue());

  if (result.toBool())
    return true;
  return tryToOpen(*character);
}

bool Doorway::tryToOpen(Character& character)
{
  auto* game  = Game::get();
  auto* level = game->getLevel();

  character.useActionPoints(2, "door");
  if (locked)
  {
    if (&character == level->getPlayer() || !character.getInventory()->count(keyName))
    {
      if (&character == level->getPlayer())
        game->appendToConsole(I18n::get()->t("messages.door-is-locked"));
      playSound(lockedSound);
    }
    else
      toggle();
  }
  else
    toggle();
  return !locked;
}

void Doorway::toggle()
{
  if (opened)
  {
    opened = false;
    playSound(closeSound);
  }
  else
  {
    opened = true;
    locked = false;
    playSound(openSound);
  }
  emit openedChanged();
  if (script && script->hasMethod("onToggle"))
    script->call("onToggle", QJSValueList() << opened);
}

void Doorway::save(QJsonObject& data) const
{
  data["destroyed"] = destroyed;
  data["opened"]    = opened;
  data["locked"]    = locked;
  data["key"]       = keyName;
  data["picklvl"]   = lockpickLevel;
  DynamicObject::save(data);
}

void Doorway::load(const QJsonObject& data)
{
  destroyed     = data["destroyed"].toBool();
  opened        = data["opened"].toBool();
  locked        = data["locked"].toBool();
  keyName       = data["key"].toString();
  lockpickLevel = data["picklvl"].toInt();
  DynamicObject::load(data);
  emit openedChanged();
  emit lockedChanged();
  emit keyNameChanged();
  emit lockpickLevelChanged();
  setAnimation(opened ? "idle-open" : "idle");
}
