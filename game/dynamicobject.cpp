#include <QRegularExpression>
#include <QDebug>
#include "dynamicobject.h"
#include "character.h"
#include "game.h"
#include "i18n.h"

DynamicObject::DynamicObject(QObject *parent) : ParentType(parent)
{
  position = QPoint(-1,-1);
  taskManager = new TaskRunner(this);
  connect(this, &DynamicObject::blocksPathChanged, this, &DynamicObject::onBlocksPathChanged);
}

DynamicObject::~DynamicObject()
{
}

void DynamicObject::setScript(const QString& name)
{
  ParentType::setScript(name);
  taskManager->setScriptController(script);
}

void DynamicObject::updateTasks(qint64 v)
{
  taskManager->update(v);
}

QString DynamicObject::getBaseName() const
{
  QRegularExpression regexp("#.*$");
  return QString(objectName).replace(regexp, "");
}

QStringList DynamicObject::getAvailableInteractions()
{
  if (script)
    return script->call("getAvailableInteractions").toVariant().toStringList();
  return QStringList() << "look";
}

bool DynamicObject::triggerInteraction(Character* character, const QString &interactionType)
{
  static const QMap<QString, QString> callbackMap = {
    {"talk-to",   "onTalkTo"},
    {"push",      "onPush"},
    {"use",       "onUse"},
    {"look",      "onLook"}
  };

  if (character && script)
  {
    const QString callback = callbackMap[interactionType];

    return script->call(callback, QJSValueList() << character->asJSValue()).toBool();
  }
  else if (interactionType == "look")
    return defaultLookInteraction();
  return false;
}

bool DynamicObject::defaultLookInteraction()
{
  const I18n* i18n = I18n::get();

  Game::get()->appendToConsole(i18n->t("inspection.item", {
    {"item", i18n->t("objects." + getBaseName())}
  }));
  return true;
}

bool DynamicObject::triggerSkillUse(Character *user, const QString &skillName)
{
  QString methodName = skillName;

  methodName[0] = methodName[0].toUpper();
  methodName = "onUse" + methodName;
  qDebug() << "Trying to call" << methodName;
  if (user && script && script->hasMethod(methodName))
    return script->call(methodName, QJSValueList() << user->asJSValue()).toBool();
  else if (user == Game::get()->getPlayer())
    Game::get()->appendToConsole("You use " + skillName + " on " + getObjectName() + ". It does nothing.");
  return false;
}

void DynamicObject::setVisible(bool value)
{
  if (visible != value)
  {
    visible = value;
    emit visibilityChanged();
  }
}

void DynamicObject::onBlocksPathChanged()
{
  auto* level = Game::get()->getLevel();
  auto* grid  = level ? level->getGrid() : nullptr;

  if (grid)
  {
    if (blocksPath)
      grid->moveObject(this, position.x(), position.y());
    else
      grid->removeObject(this);
  }
}

void DynamicObject::load(const QJsonObject& data)
{
  objectName = data["objectName"].toString();
  position.setX(data["x"].toInt()); position.setY(data["y"].toInt());
  nextPosition.setX(data["nextX"].toInt()); nextPosition.setY(data["nextY"].toInt());
  blocksPath = data["blocksPath"].toBool(true);
  emit blocksPathChanged();
  ParentType::load(data);
  taskManager->load(data);
  emit positionChanged();
}

void DynamicObject::save(QJsonObject& data) const
{
  data["objectName"] = objectName;
  data["x"] = position.x(); data["y"] = position.y();
  data["nextX"] = nextPosition.x(); data["nextY"] = nextPosition.y();
  data["blocksPath"] = blocksPath;
  ParentType::save(data);
  taskManager->save(data);
}
