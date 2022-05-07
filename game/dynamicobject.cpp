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
  connect(this, &DynamicObject::objectNameChanged, this, &DynamicObject::pathChanged);
  connect(this, &DynamicObject::parentChanged,     this, &DynamicObject::pathChanged);
}

DynamicObject::~DynamicObject()
{
}

ObjectGroup* DynamicObject::getGroup() const
{
  if (parent() && parent()->metaObject()->className() == QString("ObjectGroup"))
    return reinterpret_cast<ObjectGroup*>(parent());
  return nullptr;
}

QString DynamicObject::getPath() const
{
  ObjectGroup* group = getGroup();

  return group ? group->getPath() + '.' + getObjectName() : getObjectName();
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

QString DynamicObject::getHint() const
{
  if (script && script->hasMethod("getHint"))
    return script->call("getHint").toString();
  return QString("");
}

QStringList DynamicObject::getAvailableInteractions()
{
  if (script && script->hasMethod("getAvailableInteractions"))
    return script->call("getAvailableInteractions").toVariant().toStringList();
  if (interactive)
    return QStringList() << "look";
  return QStringList();
}

bool DynamicObject::triggerInteraction(Character* character, const QString &interactionType)
{
  static const QMap<QString, QString> callbackMap = {
    {"talk-to",   "onTalkTo"},
    {"push",      "onPush"},
    {"use",       "onUse"},
    {"look",      "onLook"}
  };
  const QString callback = callbackMap[interactionType];
  bool  handled = false;

  if (character)
    handled = scriptCall(callback, QJSValueList() << character->asJSValue()).toBool();
  if (interactionType == "look" && !handled)
    return defaultLookInteraction();
  return handled;
}

bool DynamicObject::defaultLookInteraction()
{
  const I18n* i18n = I18n::get();

  Game::get()->appendToConsole(i18n->t("inspection.item", {
    {"item", getDisplayName()}
  }));
  return true;
}

QString DynamicObject::getDisplayName() const
{
  const I18n* i18n = I18n::get();

  return i18n->t("objects." + getBaseName());
}

bool DynamicObject::triggerSkillUse(Character *user, const QString &skillName)
{
  QString methodName = skillName;
  const I18n* i18n = I18n::get();

  methodName[0] = methodName[0].toUpper();
  methodName = "onUse" + methodName;
  qDebug() << "Trying to call" << methodName;
  if (user && script && script->hasMethod(methodName))
    return script->call(methodName, QJSValueList() << user->asJSValue()).toBool();
  else if (user == Game::get()->getPlayer())
  {
    Game::get()->appendToConsole(
      i18n->t("messages.use-skill-does-nothing", {{"skillName", skillName}, {"target", getDisplayName()}})
    );
  }
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

bool DynamicObject::isDestructible() const
{
  if (script)
    return script->property("destructible").toBool();
  return false;
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

QPoint DynamicObject::getSpriteOffset() const
{
  GridComponent* level = Game::get()->getLevel();

  if (level)
    return getSpritePosition() - level->getTilePosition(getPosition());
  return QPoint(0, 0);
}

void DynamicObject::setSpriteOffset(QPoint offset)
{
  GridComponent* level = Game::get()->getLevel();

  if (level)
  {
    setRenderPosition(level->getTilePosition(getPosition()) + offset);
    emit spritePositionChanged();
  }
}

void DynamicObject::playSound(const QString& sound, qreal volume) const
{
  Game::get()->getSoundManager()->play(this, sound, volume);
}

void DynamicObject::load(const QJsonObject& data)
{
  objectName = data["objectName"].toString();
  nextPosition.setX(data["nextX"].toInt()); nextPosition.setY(data["nextY"].toInt());
  blocksPath = data["blocksPath"].toBool(true);
  interactive = data["interactive"].toBool(true);
  emit blocksPathChanged();
  taskManager->load(data);
  ParentType::load(data);
  emit positionChanged();
}

void DynamicObject::save(QJsonObject& data) const
{
  data["objectName"] = objectName;
  data["nextX"] = nextPosition.x(); data["nextY"] = nextPosition.y();
  if (!blocksPath)
    data["blocksPath"] = blocksPath;
  if (!interactive)
    data["interactive"] = false;
  ParentType::save(data);
  taskManager->save(data);
}
