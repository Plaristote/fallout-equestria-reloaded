#ifndef LEVELBASE_H
#define LEVELBASE_H

#include <QObject>
#include "game/sprite.h"
#include "game/dynamicobject.h"
#include "game/character.h"

class TileMap;

class LevelBase : public QObject
{
  Q_OBJECT
public:
  explicit LevelBase(QObject *parent = nullptr);

protected:
  QList<DynamicObject*> objects;
  QList<Character*> visibleCharacters;
  QList<Sprite*> visualEffects;
  TileMap* tilemap = nullptr;
};

#endif // LEVELBASE_H
