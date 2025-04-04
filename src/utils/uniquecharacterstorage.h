#ifndef UNIQUECHARACTERSTORAGE_H
#define UNIQUECHARACTERSTORAGE_H

#include <QObject>
#include <QQmlListProperty>
#include "game/character.h"
#include "game/level/grid.h"
#include "game/leveltask.h"

class StorageSlot;
class QJsonObject;
class TileZone;

class UniqueCharacterStorage : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QMap<QString,QList<StorageSlot*>> levelToStorage MEMBER levelToStorage CONSTANT)

public:
  explicit UniqueCharacterStorage(QObject *parent = nullptr);

  Q_INVOKABLE int saveUniqueCharactersFromLevel(LevelTask* level);
  Q_INVOKABLE int loadUniqueCharactersToLevel(LevelTask* level);
  Q_INVOKABLE bool loadCharacterToCurrentLevel(const QString& characterSheet, int x, int y, int z = NULL_FLOOR);
  Q_INVOKABLE bool loadCharacterToZone(const QString& characterSheet, const TileZone* tileZone);
  Q_INVOKABLE void saveCharacterFromCurrentLevel(Character*);
  Q_INVOKABLE void detachCharacter(Character*);
  Q_INVOKABLE Character* getCharacter(const QString& characterSheet);
  Q_INVOKABLE void log();

  void load(const QJsonObject&);
  void save(QJsonObject&);
private:
  StorageSlot* getCharacterSlot(const QString& characterSheet, bool take = false);
  StorageSlot* takeCharacterSlot(const QString& characterSheet) { return  getCharacterSlot(characterSheet, true); }
  bool loadCharacterIntoLevel(LevelTask* level, StorageSlot* characterSlot);
  bool loadCharacterIntoLevel(LevelTask* level, StorageSlot* characterSlot, Point position);
  bool saveCharacterIntoStorage(LevelTask* level, Character* character, QList<StorageSlot*>& storage);
  bool swapCharacterToStorage(Character* character, QList<StorageSlot*>& storage);

  QList<StorageSlot*>& requireLevelStorage(const QString& levelName);

signals:

private:
  QMap<QString,QList<StorageSlot*>> levelToStorage;

};

// Contains character and relative information neccessary to spawn it
class StorageSlot : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Character* storedCharacter MEMBER storedCharacter)
  Q_PROPERTY(long storedTimestampAtStorage MEMBER storedTimestampAtStorage)

public:
  explicit StorageSlot(QObject *parent = nullptr, Character* character = nullptr, long timestampAtStorage = 0);

  void load(const QJsonObject&);
  void save(QJsonObject&);
  Character* getCharacter() const { return storedCharacter; }

public:
  Character* storedCharacter;
  long storedTimestampAtStorage;
};

#endif // UNIQUECHARACTERSTORAGE_H
