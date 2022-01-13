#ifndef  LEVELBASE_H
# define LEVELBASE_H

# include <QObject>
# include "game/dynamicobject.h"
# include "game/character.h"
# include "game/objects/objectgroup.h"
# include "utils/storableobject.h"

class TileMap;

class LevelBase : public ObjectGroup
{
  Q_OBJECT

  Q_PROPERTY(Character*                      player         READ getPlayer     NOTIFY playerChanged)
  Q_PROPERTY(QQmlListProperty<DynamicObject> dynamicObjects READ getQmlObjects NOTIFY objectsChanged)
  Q_PROPERTY(QPoint                          canvasOffset   MEMBER canvasOffset NOTIFY cameraMoved)
public:
  explicit LevelBase(QObject *parent = nullptr);

  bool                       isGameEditor() const;
  Character*                 getPlayer() const;
  QList<Character*>          findCharacters(std::function<bool (Character&)> compare) const;

  void update(qint64) {}
  void load(const QJsonObject&);
  void registerAllDynamicObjects();
  virtual void registerDynamicObject(DynamicObject*);
  virtual void unregisterDynamicObject(DynamicObject*);
  virtual void registerZoneController(ControlZoneComponent*) {}
  virtual void unregisterZoneController(ControlZoneComponent*) {}

signals:
  void playerChanged();
  void cameraMoved();
  void attachedObjectsChanged();

private slots:
  void onChildrenObjectAdded(DynamicObject*);
  void onChildrenObjectRemoved(DynamicObject*);
  void onChildrenGroupAdded(ObjectGroup*);
  void onChildrenGroupRemoved(ObjectGroup*);

private:
  QQmlListProperty<DynamicObject> getQmlObjects() { return QML_QLIST_CONSTRUCTOR(DynamicObject, attachedObjects); }

protected:
  QString getScriptPath() const override { return SCRIPTS_PATH + "levels"; }

  QPoint   canvasOffset;
  QList<DynamicObject*> attachedObjects;
};

#endif // LEVELBASE_H
