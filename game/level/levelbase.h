#ifndef  LEVELBASE_H
# define LEVELBASE_H

# include <QObject>
# include "game/sprite.h"
# include "game/dynamicobject.h"
# include "game/character.h"
# include "utils/storableobject.h"

class TileMap;

class LevelBase : public StorableObject
{
  Q_OBJECT

  Q_PROPERTY(QString                         name           MEMBER name)
  Q_PROPERTY(Character*                      player         READ getPlayer     NOTIFY playerChanged)
  Q_PROPERTY(QQmlListProperty<DynamicObject> dynamicObjects READ getQmlObjects NOTIFY objectsChanged)
  Q_PROPERTY(TileMap*                        tilemap        READ getTileMap    NOTIFY tilemapReady)
public:
  explicit LevelBase(QObject *parent = nullptr);

  bool                       isGameEditor() const;
  Character*                 getPlayer() const;
  QList<Character*>          findCharacters(std::function<bool (Character&)> compare) const;
  QVector<DynamicObject*>    findDynamicObjects(std::function<bool (DynamicObject&)> compare) const;
  Q_INVOKABLE DynamicObject* getObjectByName(const QString& name) const;
  Q_INVOKABLE QJSValue       getScriptObject() const;
  TileMap*                   getTileMap() const { return tilemap; }

  void update(qint64) {}
  void registerDynamicObject(DynamicObject*);
  void unregisterDynamicObject(DynamicObject*);

signals:
  void playerChanged();
  void objectsChanged();
  void tilemapReady();

private:
  QQmlListProperty<DynamicObject> getQmlObjects() { return QML_QLIST_CONSTRUCTOR(DynamicObject, objects); }

protected:
  QString               name;
  QList<DynamicObject*> objects;
  QList<Character*>     visibleCharacters;
  TileMap*              tilemap = nullptr;
  ScriptController*     script = nullptr;
};

#endif // LEVELBASE_H
