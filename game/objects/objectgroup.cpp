#include "objectgroup.h"
#include "objectfactory.h"

ObjectGroup::ObjectGroup(QObject *parent) : StorableObject(parent)
{
  connect(this, &ObjectGroup::nameChanged,   this, &ObjectGroup::pathChanged);
  connect(this, &ObjectGroup::parentChanged, this, &ObjectGroup::pathChanged);
  connect(this, &ObjectGroup::objectAdded,   this, &ObjectGroup::objectsChanged);
  connect(this, &ObjectGroup::objectRemoved, this, &ObjectGroup::objectsChanged);
  connect(this, &ObjectGroup::groupAdded,    this, &ObjectGroup::groupsChanged);
  connect(this, &ObjectGroup::groupRemoved,  this, &ObjectGroup::groupsChanged);
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
  QStringList  parts{getName()};

  while (parentIt)
  {
    parts.prepend(parentIt->getName());
    parentIt = parentIt->getParent();
  }
  return parts.join('.');
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
  groups.push_back(group);
  emit groupAdded(group);
}

void ObjectGroup::appendObject(DynamicObject* object)
{
  objects.push_back(object);
  emit objectAdded(object);
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
