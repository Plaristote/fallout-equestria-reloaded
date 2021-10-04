#ifndef  OBJECTGROUP_H
# define OBJECTGROUP_H

#include "components/controlzone.h"
#include "../dynamicobject.h"
#include <QQmlListProperty>

class ObjectFactory;

class ObjectGroup : public ControlZoneComponent
{
  Q_OBJECT
  friend class ObjectFactory;
  typedef ControlZoneComponent ParentType;

  Q_PROPERTY(QString                         name     MEMBER name NOTIFY nameChanged)
  Q_PROPERTY(QString                         path     READ getPath NOTIFY pathChanged)
  Q_PROPERTY(QPoint                          offset   READ getOffset WRITE setOffset NOTIFY offsetChanged)
  Q_PROPERTY(QPoint                          position READ getPosition NOTIFY positionChanged)
  Q_PROPERTY(QQmlListProperty<ObjectGroup>   groups   READ getQmlGroups  NOTIFY groupsChanged)
  Q_PROPERTY(QQmlListProperty<DynamicObject> objects  READ getQmlObjects NOTIFY objectsChanged)
  Q_PROPERTY(TaskRunner*                     tasks    MEMBER taskRunner CONSTANT)
  Q_PROPERTY(ObjectGroup*                    parent   READ getParent NOTIFY parentChanged)
public:
  explicit ObjectGroup(QObject *parent = nullptr);

  QString        getPath() const;
  const QString& getName() const { return name; }
  ObjectGroup*   getParent() const;
  QPoint         getPosition() const override;
  void           setPosition(QPoint) override;
  QPoint         getOffset() const { return offset; }
  void           setOffset(QPoint);
  int            objectCount() const;
  const QList<ObjectGroup*>&   getGroups() const { return groups; }
  const QList<DynamicObject*>& getObjects() const { return objects; }
  TaskRunner*                  getTaskManager() const { return taskRunner; }

  void           load(const QJsonObject&);
  void           save(QJsonObject&) const;
  Q_INVOKABLE ObjectFactory* factory();
  Q_INVOKABLE QString validateObjectName(const QString&);

  Q_INVOKABLE QJSValue       getScriptObject() const;
  void                       eachObject(std::function<void(DynamicObject*)>) const;
  QVector<DynamicObject*>    findDynamicObjects(std::function<bool (DynamicObject&)> compare) const;
  QVector<DynamicObject*>    allDynamicObjects() const;
  QList<ObjectGroup*>        allObjectGroups() const;
  void                       collectObjects(QVector<DynamicObject*>&) const;
  void                       collectObjects(std::function<bool (DynamicObject&)> compare, QVector<DynamicObject*>&) const;
  Q_INVOKABLE DynamicObject* findObject(const QString& path) const { return find<DynamicObject>(path, &ObjectGroup::getObjectByName); }
  Q_INVOKABLE ObjectGroup*   findGroup(const QString& path)  const { return find<ObjectGroup>  (path, &ObjectGroup::getGroupByName); }
  Q_INVOKABLE DynamicObject* getObjectByName(const QString& name) const;
  Q_INVOKABLE ObjectGroup*   getGroupByName(const QString& name) const;
  Q_INVOKABLE void           appendGroup(ObjectGroup*);
  Q_INVOKABLE bool           deleteGroup(ObjectGroup* group) { return detachGroup(group, true); }
  bool                       detachGroup(ObjectGroup*, bool destroy = false);
  Q_INVOKABLE void           appendObject(DynamicObject*);
  Q_INVOKABLE bool           deleteObject(DynamicObject* o) { return detachObject(o, true); }
  bool                       detachObject(DynamicObject* o, bool destroy = false);

signals:
  void nameChanged();
  void pathChanged();
  void offsetChanged();
  void objectAdded(DynamicObject* addedObject);
  void objectRemoved(DynamicObject* removedObject);
  void groupAdded(ObjectGroup* addedGroup);
  void groupRemoved(ObjectGroup* removedGroup);
  void objectsChanged();
  void groupsChanged();
  void parentChanged();

private:
  QQmlListProperty<ObjectGroup>   getQmlGroups()  { return QML_QLIST_CONSTRUCTOR(ObjectGroup,   groups);  }
  QQmlListProperty<DynamicObject> getQmlObjects() { return QML_QLIST_CONSTRUCTOR(DynamicObject, objects); }

  template<typename RESULT_TYPE>
  RESULT_TYPE* find(const QString& path, RESULT_TYPE* (ObjectGroup::*getter)(const QString&) const) const
  {
    if (path.indexOf('.') >= 0)
    {
      QStringList  parts = path.split('.');
      ObjectGroup* group = getGroupByName(parts.first());

      if (group)
      {
        parts.removeFirst();
        return group->find<RESULT_TYPE>(parts.join('.'), getter);
      }
      return nullptr;
    }
    return (this->*getter)(path);
  }

protected:
  QString               name;
  QPoint                offset;
  QList<ObjectGroup*>   groups;
  QList<DynamicObject*> objects;
  TaskRunner*           taskRunner = nullptr;
  ObjectFactory*        _factory = nullptr;
};

#endif // OBJECTGROUP_H
