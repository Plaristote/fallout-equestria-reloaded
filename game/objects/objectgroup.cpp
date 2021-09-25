#include "objectgroup.h"
#include "objectfactory.h"
#include <QJsonObject>
#include <QJsonArray>

ObjectGroup::ObjectGroup(QObject *parent) : ParentType(parent)
{
  connect(this, &ObjectGroup::nameChanged,   this, &ObjectGroup::pathChanged);
  connect(this, &ObjectGroup::parentChanged, this, &ObjectGroup::pathChanged);
  connect(this, &ObjectGroup::objectAdded,   this, &ObjectGroup::objectsChanged);
  connect(this, &ObjectGroup::objectRemoved, this, &ObjectGroup::objectsChanged);
  connect(this, &ObjectGroup::groupAdded,    this, &ObjectGroup::groupsChanged);
  connect(this, &ObjectGroup::groupRemoved,  this, &ObjectGroup::groupsChanged);
}

void ObjectGroup::load(const QJsonObject& data)
{
  ParentType::load(data);
  name = data["name"].toString();
  for (QJsonValue groupData : data["groups"].toArray())
  {
    ObjectGroup* childGroup = new ObjectGroup(this);

    childGroup->load(data);
    appendGroup(childGroup);
  }
  for (QJsonValue objectData : data["objects"].toArray())
  {
    DynamicObject* childObject = factory()->loadFromJson(objectData.toObject());

    if (childObject)
      appendObject(childObject);
    else
      qDebug() << "/!\\ ObjectGroup: failed to load object:" << getPath() << objectData["name"].toString();
  }
  emit nameChanged();
}

void ObjectGroup::save(QJsonObject& data) const
{
  QJsonArray jsonGroups, jsonObjects;

  ParentType::save(data);
  for (ObjectGroup* group : groups)
  {
    QJsonObject jsonGroup;

    group->save(jsonGroup);
    jsonGroups << jsonGroup;
  }
  for (DynamicObject* object : objects)
  {
    QJsonObject jsonObject{{"type", object->metaObject()->className()}};

    object->save(jsonObject);
    jsonObjects << jsonObject;
  }
  data["name"]    = name;
  data["groups"]  = jsonGroups;
  data["objects"] = jsonObjects;
}

ObjectFactory* ObjectGroup::factory()
{
  if (!_factory)
    _factory = new ObjectFactory(this);
  return _factory;
}

QString ObjectGroup::validateObjectName(const QString& name)
{
  if (name.indexOf('.') >= 0)
    return QString("Invalid object name: contains forbidden character '.'");
  if (getObjectByName(name))
    return QString("Object name must be unique within an object group");
  return QString();
}

QJSValue ObjectGroup::getScriptObject() const
{
  if (script)
    return script->getObject();
  return QJSValue();
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

void ObjectGroup::setOffset(QPoint newOffset)
{
  QPoint diff = newOffset - offset;

  for (ObjectGroup* group : groups)
    group->setOffset(group->getOffset() + diff);
  for (DynamicObject* object : objects)
  {
    object->getPosition();
  }
  emit offsetChanged();
}

void ObjectGroup::eachObject(std::function<void (DynamicObject *)> callback) const
{
  for (ObjectGroup* group : groups)
    group->eachObject(callback);
  for (DynamicObject* object : objects)
    callback(object);
}

QVector<DynamicObject*> ObjectGroup::findDynamicObjects(std::function<bool (DynamicObject &)> compare) const
{
  QVector<DynamicObject*> results;

  results.reserve(objectCount());
  for (ObjectGroup* group : groups)
    group->collectObjects(compare, results);
  collectObjects(compare, results);
  return results;
}

void ObjectGroup::collectObjects(std::function<bool (DynamicObject &)> compare, QVector<DynamicObject*>& results) const
{
  results.reserve(objects.size());
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
  group->setParent(this);
  groups.push_back(group);
  emit groupAdded(group);
  emit group->parentChanged();
}

void ObjectGroup::appendObject(DynamicObject* object)
{
  object->setParent(this);
  objects.push_back(object);
  emit objectAdded(object);
  emit object->parentChanged();
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

bool ObjectGroup::detachObject(DynamicObject* o, bool destroy)
{
  if (objects.indexOf(o) >= 0)
  {
    objects.removeOne(o);
    if (destroy)
      o->deleteLater();
    emit objectRemoved(o);
    return true;
  }
  return recursivelyDetachChild(groups, o, destroy, &ObjectGroup::detachObject);
}
