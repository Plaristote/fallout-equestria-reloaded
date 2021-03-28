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
}

void Doorway::updateAccessPath()
{
  if (hasControlZone())
    toggleZoneBlocked(!opened);
  else
  {
    blocksPath = !opened;
    emit blocksPathChanged();
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

QStringList Doorway::getAvailableInteractions()
{
  if (script && script->hasMethod("getAvailableInteractions"))
    return script->call("getAvailableInteractions").toVariant().toStringList();
  return QStringList() << "use" << "look" << "use-object" << "use-skill";
}

bool Doorway::triggerInteraction(Character *character, const QString &interactionType)
{
  if (interactionType == "use" && !script->hasMethod("onUse"))
    return onUse(character);
  return DynamicObject::triggerInteraction(character, interactionType);
}

bool Doorway::onUse(Character* character)
{
  auto* game  = Game::get();
  auto* level = game->getLevel();
  auto* grid  = level->getGrid();
  QPoint position = getPosition();

  if (locked)
  {
    if (character == level->getPlayer())
      game->appendToConsole(I18n::get()->t("messages.door-is-locked"));
    return false;
  }
  if (opened)
  {
    if (!grid->isOccupied(position.x(), position.y()))
    {
      opened = false;
      emit openedChanged();
      return true;
    }
    else if (character == level->getPlayer())
      game->appendToConsole(I18n::get()->t("messages.door-is-blocked"));
    return false;
  }
  else
    opened = true;
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
