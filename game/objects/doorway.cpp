#include "doorway.h"
#include "tilemap/tilezone.h"
#include "game.h"
#include <QJsonArray>

Doorway::Doorway(QObject* parent) : DynamicObject(parent)
{
  connect(this, &Doorway::openedChanged, [this]()
  {
    if (controlZone)
      controlZone->setAccessBlocked(!opened);
    else
    {
      blocksPath = !opened;
      emit blocksPathChanged();
    }
  });
}

QStringList Doorway::getAvailableInteractions()
{
  if (script && script->hasMethod("getAvailableInteractions"))
    return script->call("getAvailableInteractions").toVariant().toStringList();
  return QStringList() << "use" << "use-skill";
}

QPoint Doorway::getInteractionPosition() const
{
  auto* level = Game::get()->getLevel();

  if (level && level->getPlayer())
  {
    QList<QPoint> path;

    if (level->getGrid()->findPath(level->getPlayer()->getPosition(), getPosition(), path))
    {
      if (path.length() > 1)
        return path.at(path.length() - 2);
      return level->getPlayer()->getPosition();
    }
    qDebug() << "Character::getInteractionPosition: No path to reach character" << getObjectName();
  }
  else
    qDebug() << "Character::getInteractionPosition: called outside level";
  return getPosition();
}

bool Doorway::triggerInteraction(Character *character, const QString &interactionType)
{
  qDebug() << "triggerInteraction on doorway" << interactionType;
  if (interactionType == "use")
  {
    auto* level = Game::get()->getLevel();
    auto* grid  = level->getGrid();
    QPoint position = getPosition();

    qDebug() << "pas glop";
    if (locked)
    {
      if (character == level->getPlayer())
        level->displayConsoleMessage("It's locked.");
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
        level->displayConsoleMessage("Something's in the way.");
      return false;
    }
    else
      opened = true;
    emit openedChanged();
    return true;
  }
  return DynamicObject::triggerInteraction(character, interactionType);
}

void Doorway::save(QJsonObject& data) const
{
  data["opened"]  = opened;
  data["locked"]  = locked;
  data["key"]     = keyName;
  data["picklvl"] = lockpickLevel;
  DynamicObject::save(data);
}

void Doorway::load(const QJsonObject& data)
{
  opened        = data["opened"].toBool();
  locked        = data["locked"].toBool();
  keyName       = data["key"].toString();
  lockpickLevel = data["picklvl"].toInt();
  DynamicObject::load(data);
  emit openedChanged();
  emit lockedChanged();
  emit keyNameChanged();
  emit lockpickLevelChanged();
}
