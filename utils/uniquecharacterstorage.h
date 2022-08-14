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

  Q_PROPERTY(QMap<QString,QList<StorageSlot*>> levelToStorage MEMBER levelToStorage CONSTANT)

public:
  explicit UniqueCharacterStorage(QObject *parent = nullptr);

  Q_INVOKABLE int saveUniqueCharactersFromLevel(GridComponent* level);
  Q_INVOKABLE int loadUniqueCharactersToLevel(GridComponent* level);
  Q_INVOKABLE void log();


signals:

private:
  QMap<QString,QList<StorageSlot*>> levelToStorage;

};

class StorageSlot : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Character* storedCharacter MEMBER storedCharacter)
  Q_PROPERTY(long storedTimestampAtStorage MEMBER storedTimestampAtStorage)

public:
  explicit StorageSlot(QObject *parent = nullptr, Character* character = nullptr, long timestampAtStorage = 0);

public:
  Character* storedCharacter;
  long storedTimestampAtStorage;
};

#endif // UNIQUECHARACTERSTORAGE_H
