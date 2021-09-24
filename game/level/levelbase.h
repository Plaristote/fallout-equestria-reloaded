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
  Q_PROPERTY(TileMap*                        tilemap        READ getTileMap    NOTIFY tilemapReady)
  Q_PROPERTY(QPoint                          canvasOffset   MEMBER canvasOffset NOTIFY cameraMoved)
public:
  explicit LevelBase(QObject *parent = nullptr);

  bool                       isGameEditor() const;
  Character*                 getPlayer() const;
  QList<Character*>          findCharacters(std::function<bool (Character&)> compare) const;
  TileMap*                   getTileMap() const { return tilemap; }

  void update(qint64) {}
  void load(const QJsonObject&);
  void registerAllDynamicObjects();
  void registerDynamicObject(DynamicObject*);
  void unregisterDynamicObject(DynamicObject*);

signals:
  void playerChanged();
  void tilemapReady();
  void cameraMoved();

private slots:
  void onChildrenObjectAdded(DynamicObject*);
  void onChildrenObjectRemoved(DynamicObject*);
  void onChildrenGroupAdded(ObjectGroup*);
  void onChildrenGroupRemoved(ObjectGroup*);

private:
  QQmlListProperty<DynamicObject> getQmlObjects() { return QML_QLIST_CONSTRUCTOR(DynamicObject, objects); }

protected:
  QString getScriptPath() const { return SCRIPTS_PATH + "levels"; }

  TileMap* tilemap = nullptr;
  QPoint   canvasOffset;
};

#endif // LEVELBASE_H
