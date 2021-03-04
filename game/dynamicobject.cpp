#include "dynamicobject.h"
#include "character.h"
#include "game.h"
#include "tilemap/tilezone.h"
#include <QJsonArray>

DynamicObject::DynamicObject(QObject *parent) : Sprite(parent)
{
  taskManager = new TaskRunner(this);
  connect(this, &DynamicObject::controlZoneAdded,   this, &DynamicObject::controlZoneChanged);
  connect(this, &DynamicObject::controlZoneRemoved, this, &DynamicObject::controlZoneChanged);
}

DynamicObject::~DynamicObject()
{
  if (script)
    delete script;
}

void DynamicObject::setScript(const QString& name)
{
  if (script)
    delete script;
  scriptName = name;
  script     = new ScriptController(getScriptPath() + '/' + name, this);
  //taskManager->setScriptController(script);
}

QStringList DynamicObject::getAvailableInteractions()
{
  if (script)
  {
    qDebug() << "Calling get available interactions on " << getObjectName();
    return script->call("getAvailableInteractions").toVariant().toStringList();
  }
  else
    qDebug() << "/!\\ Missing script on object " << getObjectName();
  return QStringList();
}

bool DynamicObject::triggerInteraction(Character* character, const QString &interactionType)
{
  static const QMap<QString, QString> callbackMap = {
    {"talk-to",   "onTalkTo"},
    {"use",       "onUse"},
    {"use-skill", "onUseSkill"},
    {"use-magic", "onUseMagic"},
    {"look",      "onLook"}
  };

  if (script)
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(character);
    return script->call(callbackMap[interactionType], args).toBool();
  }
  return false;
}

bool DynamicObject::triggerSkillUse(Character *user, const QString &skillName)
{
  QString methodName = skillName;

  methodName[0] = methodName[0].toUpper();
  methodName = "onUse" + methodName;
  if (script && script->hasMethod(methodName))
  {
    QJSValueList args;

    args << Game::get()->getScriptEngine().newQObject(user);
    return script->call(methodName, args).toBool();
  }
  else if (user == Game::get()->getPlayer())
    Game::get()->appendToConsole("You use " + skillName + " on " + getObjectName() + ". It does nothing.");
  return false;
}

void DynamicObject::scriptCall(const QString& method, const QString& message)
{
  if (script)
    script->call(method, QJSValueList() << message);
}

void DynamicObject::update(qint64 delta)
{
  Sprite::update(delta);
  taskManager->update(delta);
}

TileZone* DynamicObject::addControlZone()
{
  if (controlZone == nullptr)
  {
    controlZone = new TileZone(this);
    emit controlZoneAdded(controlZone);
  }
  return controlZone;
}

void DynamicObject::removeControlZone()
{
  if (controlZone != nullptr)
  {
    delete controlZone;
    emit controlZoneRemoved(controlZone);
  }
  controlZone = nullptr;
}

void DynamicObject::load(const QJsonObject& data)
{
  objectName = data["objectName"].toString();
  position.setX(data["x"].toInt()); position.setY(data["y"].toInt());
  nextPosition.setX(data["nextX"].toInt()); nextPosition.setY(data["nextY"].toInt());
  interactionPosition.setX(data["intX"].toInt()); interactionPosition.setY(data["intY"].toInt());
  if (data["zone"].isArray())
  {
    controlZone = controlZone ? controlZone : new TileZone(this);
    for (QJsonValue posValue : data["zone"].toArray())
    {
      QJsonArray posArray(posValue.toArray());

      controlZone->addPosition(QPoint(posArray[0].toInt(), posArray[1].toInt()));
    }
    emit controlZoneChanged();
  }
  currentZone = data["currentZone"].toString();
  scriptName  = data["script"].toString();
  dataStore   = data["dataStore"].toObject();
  Sprite::load(data);
  setScript(scriptName);
  taskManager->load(data);
}

void DynamicObject::save(QJsonObject& data) const
{
  data["objectName"] = objectName;
  data["x"] = position.x(); data["y"] = position.y();
  data["nextX"] = nextPosition.x(); data["nextY"] = nextPosition.y();
  data["intX"] = interactionPosition.x(); data["intY"] = interactionPosition.y();
  if (controlZone)
  {
    QJsonArray zoneArray;

    for (QPoint position : controlZone->getPositions())
    {
      QJsonArray posArray;

      posArray << position.x() << position.y();
      zoneArray << posArray;
    }
    data["zone"] = zoneArray;
  }
  data["currentZone"] = currentZone;
  data["script"]      = scriptName;
  data["dataStore"]   = dataStore;
  Sprite::save(data);
  taskManager->save(data);
}
