#ifndef UNIQUECHARACTERSTORAGE_H
#define UNIQUECHARACTERSTORAGE_H

#include <QObject>
#include <QQmlListProperty>
#include "game/character.h"
#include "game/level/grid.h"
class StorageSlot;

class UniqueCharacterStorage : public QObject
{
  Q_OBJECT

public:
  explicit UniqueCharacterStorage(QObject *parent = nullptr);

  Q_INVOKABLE int saveUniqueCharactersFromLevel(GridComponent* level);
  Q_INVOKABLE int loadUniqueCharactersToLevel(GridComponent* level);

signals:

private:
  QMap<QString,QList<StorageSlot*>> levelToStorage;

};

class StorageSlot : public QObject
{
  Q_OBJECT

public:
  explicit StorageSlot(QObject *parent = nullptr, Character* character = nullptr, QPoint position=QPoint());

public:
  Character* storedCharacter;
  QPoint storedPosition;

};

#endif // UNIQUECHARACTERSTORAGE_H
