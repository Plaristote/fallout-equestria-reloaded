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
  openSound = closeSound = "door-open";
  lockedSound = "door-locked";
  tileConnections.reserve(8);
}

void Doorway::removeTileConnections()
{
  for (void* caseContent : qAsConst(tileConnections))
    reinterpret_cast<LevelGrid::CaseConnection*>(caseContent)->doorway = nullptr;
  tileConnections.clear();
}

void Doorway::updateTileConnections()
{
  QPoint                       origin = getPosition();
  LevelGrid*                   grid = Game::get()->getLevel()->getGrid();
  LevelGrid::CaseContent*      doorwayCase = grid->getGridCase(origin);
  static const QVector<QPoint> upperTargets {QPoint(-1, -1), QPoint(0, -1), QPoint(1, -1)};
  static const QVector<QPoint> bottomTargets{QPoint(-1,  1), QPoint(0,  1), QPoint(1,  1)};
  static const QVector<QPoint> leftTargets  {QPoint(-1, -1), QPoint(-1, 0), QPoint(-1, 1)};
  static const QVector<QPoint> rightTargets {QPoint(1,  -1), QPoint(1,  0), QPoint(1,  1)};
  QVector<QPoint>              targets;

  removeTileConnections();
  if (getOrientation() == UpperDir || getOrientation() == BottomDir)
    targets << upperTargets << bottomTargets;
  else if (getOrientation() == LeftDir || getOrientation() == RightDir)
    targets << leftTargets << rightTargets;
  else
    qDebug() << "Unsupported Doorway direction" << getOrientationName() << "for" << getObjectName();
  for (const QPoint& target : targets)
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
    LevelGrid*              grid = Game::get()->getLevel()->getGrid();
    LevelGrid::CaseContent* doorwayCase = grid->getGridCase(getPosition());

    if (doorwayCase)
      doorwayCase->cover = opened ? 75 : 100;
  }
}

void Doorway::updateAnimation()
{
  if (!destroyed)
    setAnimation(opened ? "open" : "close");
  else
    setAnimation("destroyed");
}

bool Doorway::isDestructible() const
{
  if (script)
    return script->property("destructible").toBool();
  return false;
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
    emit destroyedChanged();
    emit openedChanged();
  }
  return success;
}

bool Doorway::onGoThrough(Character* character)
{
  if (!opened && canGoThrough(character))
  {
    auto* level = Game::get()->getLevel();

    character->useActionPoints(2, "doorway");
    opened = true;
    level->getSoundManager()->play(openSound);
    emit openedChanged();
    return true;
  }
  return opened;
}

bool Doorway::canGoThrough(Character* character) const
{
  if (character == Game::get()->getPlayer())
    return opened;
  if (!opened)
  {
    if (script && script->hasMethod("canGoThrough"))
    {
      QJSValue result = script->call("canGoThrough", QJSValueList() << character->asJSValue());

      if (result.isBool())
        return result.toBool();
    }
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
  auto* game  = Game::get();
  auto* level = game->getLevel();
  auto* grid  = level->getGrid();
  QPoint position = getPosition();

  if (script->hasMethod("onUse") && script->call("onUse", QJSValueList() << character->asJSValue()).toBool())
    return true;
  character->useActionPoints(2, "door");
  if (locked)
  {
    if (character == level->getPlayer())
      game->appendToConsole(I18n::get()->t("messages.door-is-locked"));
    level->getSoundManager()->play(lockedSound);
    return false;
  }
  if (opened)
  {
    if (!grid->isOccupied(position.x(), position.y()))
    {
      opened = false;
      level->getSoundManager()->play(closeSound);
      emit openedChanged();
      return true;
    }
    else if (character == level->getPlayer())
      game->appendToConsole(I18n::get()->t("messages.door-is-blocked"));
    level->getSoundManager()->play(lockedSound);
    return false;
  }
  else
  {
    opened = true;
    level->getSoundManager()->play(openSound);
  }
  emit openedChanged();
  return true;
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
