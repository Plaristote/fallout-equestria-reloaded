#include "objectgroup.h"
#include "objectfactory.h"
#include "game.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>

template<typename OBJECT_TYPE>
static QString makeUniqueName(QString base, const ObjectGroup* group, OBJECT_TYPE* (ObjectGroup::*getter)(const QString&) const)
{
  if ((group->*getter)(base) != nullptr)
  {
    QString newName = base.replace(QRegularExpression("#[0-9]+$"), "");
    unsigned int i = 0;

    do {
      newName = base + '#' + QString::number(++i);
    } while ((group->*getter)(newName) != nullptr);
    return newName;
  }
  return base;
}

static QString validateName(const QString& name, bool unique)
{
  if (name.indexOf('.') >= 0)
    return QString("Invalid object name: contains forbidden character '.'");
  if (!unique)
    return QString("Object name must be unique within an object group");
  return QString();
}

ObjectGroup::ObjectGroup(QObject *parent) : ParentType(parent)
{
  taskRunner = new TaskRunner(this);
  connect(this, &ObjectGroup::offsetChanged, this, &ObjectGroup::positionChanged);
  connect(this, &ObjectGroup::parentChanged, this, &ObjectGroup::positionChanged);
  connect(this, &ObjectGroup::nameChanged,   this, &ObjectGroup::pathChanged);
  connect(this, &ObjectGroup::parentChanged, this, &ObjectGroup::pathChanged);
  connect(this, &ObjectGroup::objectAdded,   this, &ObjectGroup::objectsChanged);
  connect(this, &ObjectGroup::objectRemoved, this, &ObjectGroup::objectsChanged);
  connect(this, &ObjectGroup::groupAdded,    this, &ObjectGroup::groupsChanged);
  connect(this, &ObjectGroup::groupRemoved,  this, &ObjectGroup::groupsChanged);
  connect(this, &ObjectGroup::floorChanged,  this, &ObjectGroup::propagateFloorUpdate);
}

void ObjectGroup::load(const QJsonObject& data)
{
  const QJsonArray groupsData  = data["groups"].toArray();
  const QJsonArray objectsData = data["objects"].toArray();

  ParentType::load(data);
  name = data["name"].toString();
  offset.setX(data["ox"].toInt(0));
  offset.setY(data["oy"].toInt(0));
  taskRunner->setScriptController(script);
  taskRunner->load(data["tasks"].toObject());
  for (const QJsonValue& groupData : groupsData)
  {
    ObjectGroup* childGroup = new ObjectGroup(this);

    childGroup->load(groupData.toObject());
    appendGroup(childGroup);
  }
  for (const QJsonValue& objectData : objectsData)
  {
    DynamicObject* childObject = factory()->loadFromJson(objectData.toObject());

    if (!childObject)
      qDebug() << "/!\\ ObjectGroup: failed to load object:" << getPath() << objectData;
  }
  emit nameChanged();
}

void ObjectGroup::save(QJsonObject& data) const
{
  QJsonArray  jsonGroups, jsonObjects;
  QJsonObject taskData;
  auto*       party = Game::get()->getPlayerParty();

  ParentType::save(data);
  taskRunner->save(taskData);
  for (ObjectGroup* group : groups)
  {
    QJsonObject jsonGroup;

    group->save(jsonGroup);
    jsonGroups << jsonGroup;
  }
  for (DynamicObject* object : objects)
  {
    QJsonObject jsonObject{{"type", object->metaObject()->className()}};

    if (object->isCharacter() && party->containsCharacter(reinterpret_cast<Character*>(object)))
      continue ;
    object->save(jsonObject);
    jsonObjects << jsonObject;
  }
  data["name"]    = name;
  data["groups"]  = jsonGroups;
  data["objects"] = jsonObjects;
  data["ox"] = offset.x();
  data["oy"] = offset.y();
  data.insert("tasks", taskData);
}

ObjectFactory* ObjectGroup::factory()
{
  if (!_factory)
    _factory = new ObjectFactory(this);
  return _factory;
}

QString ObjectGroup::validateObjectName(const QString& name) const
{
  return validateName(name, getObjectByName(name) == nullptr);
}

QString ObjectGroup::validateGroupName(const QString& name) const
{
  return validateName(name, getGroupByName(name) == nullptr);
}

QString ObjectGroup::getRelativePath(const ObjectGroup& group) const
{
  QString path = getPath();
  QString from = group.getPath();

  if (from.length() > 0)
    return path.right(path.length() - (from.length() + 1));
  return path;
}

QJSValue ObjectGroup::find(QJSValue filter) const
{
  return filter.isString()
    ? findFromExpression(filter.toString())
    : findFromFilter(filter);
}

QString jsErrorBacktrace(QJSValue retval);

template<typename OBJECT_TYPE>
static std::function<void(OBJECT_TYPE*)> makeFilterLambda(QJSValue filter, QJSValue& result, QJSValue& push)
{
  return [&filter, &push, &result](OBJECT_TYPE* object)
  {
    QJSValueList params{object->asJSValue()};
    QJSValue retval = filter.call(params);

    if (retval.isError())
      jsErrorBacktrace(retval);
    else if (retval.toBool())
      push.callWithInstance(result, params);
  };
}

template<typename OBJECT_TYPE>
static std::function<void(OBJECT_TYPE*)> makeExpressionLambda(const ObjectGroup* group, const QRegularExpression& regex, QJSValue& result, QJSValue& push)
{
  return [&regex, &result, &push, group](OBJECT_TYPE* object)
  {
    if (regex.match(object->getRelativePath(*group)).hasMatch())
      push.callWithInstance(result, QJSValueList() << object->asJSValue());
  };
}

QJSValue ObjectGroup::findFromFilter(QJSValue filter) const
{
  QJSValue result = Game::get()->getScriptEngine().newArray();
  QJSValue push = result.property("push");

  eachGroup(makeFilterLambda<ObjectGroup>(filter, result, push));
  eachObject(makeFilterLambda<DynamicObject>(filter, result, push));
  return result;
}

QJSValue ObjectGroup::findFromExpression(QString expression) const
{
  QRegularExpression regex('^' + expression
    .replace('.', "\\.")
    .replace("**", ".*")
    .replace('*', "[^.]+") + '$');
  QVector<DynamicObject*> objects;
  QJSValue result = Game::get()->getScriptEngine().newArray();
  QJSValue push = result.property("push");

  eachGroup(makeExpressionLambda<ObjectGroup>(this, regex, result, push));
  eachObject(makeExpressionLambda<DynamicObject>(this, regex, result, push));
  return result;
}

ObjectGroup* ObjectGroup::getParent() const
{
  return parent() && parent()->metaObject()->inherits(ObjectGroup().metaObject())
    ? reinterpret_cast<ObjectGroup*>(parent()) : nullptr;
}

QString ObjectGroup::getPath() const
{
  ObjectGroup* parentIt = getParent();
  QString path;

  if (parentIt)
    path = parentIt->getPath();
  if (path.length() > 0)
    path += '.';
  if (parentIt)
    path += getName();
  return path;
}

QPoint ObjectGroup::getPosition() const
{
  ObjectGroup* parent = getParent();

  return parent ? parent->getPosition() + offset : offset;
}

void ObjectGroup::setPosition(QPoint value)
{
  ObjectGroup* parent = getParent();
  QPoint newOffset = parent ? value - parent->getPosition() : value;

  if (offset != newOffset)
  {
    offset = newOffset;
    emit offsetChanged();
  }
}

static void adjustOffsetWithLevel(LevelTask* level, DynamicObject* object, QPoint movement)
{
  auto newPosition = object->getPosition() + movement;

  if (object->isCharacter())
    level->setCharacterPosition(reinterpret_cast<Character*>(object), newPosition.x(), newPosition.y());
  else
    level->setObjectPosition(object, newPosition.x(), newPosition.y());
}

static void adjustOffsetWithoutLevel(DynamicObject* object, QPoint movement)
{
  object->setPosition(object->getPosition() + movement);
}

void ObjectGroup::setOffset(QPoint newOffset)
{
  using namespace std;

  if (newOffset != offset)
  {
    QPoint     diff = newOffset - offset;
    LevelTask* level = LevelTask::get();
    auto       objectAdjustStrategy = level
      ? function<void (DynamicObject*)>(bind(&adjustOffsetWithLevel, level, placeholders::_1, diff))
      : function<void (DynamicObject*)>(bind(&adjustOffsetWithoutLevel, placeholders::_1, diff));

    offset = newOffset;
    for (ObjectGroup* group : groups)
      group->setOffset(group->getOffset() + diff);
    for (DynamicObject* object : objects)
      objectAdjustStrategy(object);
    emit offsetChanged();
  }
}

void ObjectGroup::propagateFloorUpdate()
{
  for (ObjectGroup* group : groups)
    group->setCurrentFloor(getCurrentFloor());
  for (DynamicObject* object : objects)
    object->setCurrentFloor(getCurrentFloor());
}

void ObjectGroup::eachObject(std::function<void (DynamicObject *)> callback) const
{
  for (ObjectGroup* group : groups)
    group->eachObject(callback);
  for (DynamicObject* object : objects)
    callback(object);
}

void ObjectGroup::eachGroup(std::function<void (ObjectGroup*)> callback) const
{
  for (ObjectGroup* group : groups)
  {
    callback(group);
    group->eachGroup(callback);
  }
}

QVector<DynamicObject*> ObjectGroup::allDynamicObjects() const
{
  QVector<DynamicObject*> results;

  results.reserve(512);
  for (ObjectGroup* group : groups)
    results << group->allDynamicObjects();
  collectObjects(results);
  return results;
}

QList<ObjectGroup*> ObjectGroup::allObjectGroups() const
{
  QList<ObjectGroup*> results;

  results.reserve(512);
  results << groups;
  for (ObjectGroup* group : groups)
    results << group->allObjectGroups();
  return results;
}

QVector<DynamicObject*> ObjectGroup::findDynamicObjects(std::function<bool (DynamicObject &)> compare) const
{
  QVector<DynamicObject*> results;

  results.reserve(objectCount());
  for (ObjectGroup* group : groups)
    results << group->findDynamicObjects(compare);
  collectObjects(compare, results);
  return results;
}

DynamicObject* ObjectGroup::findObject(std::function<bool (DynamicObject&)> compare) const
{
  for (DynamicObject* object : objects)
  {
    if (compare(*object))
      return object;
  }
  for (ObjectGroup* group : groups)
  {
    DynamicObject* result = group->findObject(compare);

    if (result)
      return result;
  }
  return nullptr;
}

void ObjectGroup::collectObjects(QVector<DynamicObject*>& results) const
{
  results.reserve(results.size() + objects.size());
  for (DynamicObject* object : objects)
    results << object;
}

void ObjectGroup::collectObjects(std::function<bool (DynamicObject &)> compare, QVector<DynamicObject*>& results) const
{
  results.reserve(results.size() + objects.size());
  for (DynamicObject* object : objects)
  {
    if (compare(*object))
      results << object;
  }
}

int ObjectGroup::objectCount() const
{
  int total = objects.size();

  for (ObjectGroup* child : groups)
    total += child->objectCount();
  return total;
}

DynamicObject* ObjectGroup::getObjectByName(const QString &name) const
{
  for (DynamicObject* object : objects)
  {
    if (object->getObjectName() == name)
      return object;
  }
  return nullptr;
}

ObjectGroup* ObjectGroup::getGroupByName(const QString &name) const
{
  for (ObjectGroup* group : groups)
  {
    if (group->getName() == name)
      return group;
  }
  return nullptr;
}

void ObjectGroup::appendGroup(ObjectGroup* group)
{
  if (!groups.contains(group))
  {
    QString name = makeUniqueName(group->getName(), this, &ObjectGroup::getGroupByName);

    group->setProperty("name", name);
    group->setParent(this);
    groups.push_back(group);
    emit groupAdded(group);
    emit group->parentChanged();
  }
}

void ObjectGroup::appendObject(DynamicObject* object)
{
  if (!objects.contains(object))
  {
    QString name = makeUniqueName(object->getObjectName(), this, &ObjectGroup::getObjectByName);

    object->setObjectName(name);
    object->setParent(this);
    objects.push_back(object);
    emit objectAdded(object);
    emit object->parentChanged();
  }
}

template<typename TYPE>
static bool recursivelyDetachChild(const QList<ObjectGroup*>& groups, TYPE* object, bool destroy, bool (ObjectGroup::*method)(TYPE*, bool))
{
  for (ObjectGroup* group : groups)
  {
    if ((group->*method)(object, destroy))
      return true;
  }
  return false;
}

bool ObjectGroup::detachGroup(ObjectGroup* o, bool destroy)
{
  if (groups.indexOf(o) >= 0)
  {
    groups.removeOne(o);
    if (destroy)
      o->deleteLater();
    emit groupRemoved(o);
    return true;
  }
  return recursivelyDetachChild(groups, o, destroy, &ObjectGroup::detachGroup);
}

class DynamicObjectDeleter
{
public:
  static void deleteObject(DynamicObject* object)
  {
    auto* deleter = new QTimer;
    object->setParent(nullptr);
    emit object->beforeDestroy(object);
    QObject::connect(deleter, &QTimer::timeout, deleter, [object, deleter]()
    {
      qDebug() << "Deleting object now";
      delete object;
      deleter->deleteLater();
      qDebug() << "Deleting object done";
    });
    QObject::connect(object, &QObject::destroyed, [deleter]()
    {
      deleter->stop();
      deleter->deleteLater();
    });
    deleter->start(15432);
  }
};

bool ObjectGroup::detachObject(DynamicObject* o, bool destroy)
{
  if (objects.indexOf(o) >= 0)
  {
    objects.removeOne(o);
    if (destroy)
      DynamicObjectDeleter::deleteObject(o);
    emit objectRemoved(o);
    return true;
  }
  return recursivelyDetachChild(groups, o, destroy, &ObjectGroup::detachObject);
}
