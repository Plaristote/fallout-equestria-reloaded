#ifndef  LEVELTASK_H
# define LEVELTASK_H

# include <QObject>
# include <QElapsedTimer>
# include <QQmlListProperty>
# include "tilemap/tilemap.h"
# include "levelgrid.h"
# include "timermanager.h"
# include "soundmanager.h"

# include "character.h"
# include "dynamicobject.h"

# include "level/combat.h"

class Doorway;
class CharacterParty;
class CharacterDialog;
class DataEngine;

class LevelTask : public CombatComponent
{
  Q_OBJECT

  Q_PROPERTY(QString    name MEMBER name)
  Q_PROPERTY(bool       paused  MEMBER paused NOTIFY pausedChanged)
  Q_PROPERTY(TileMap*   tilemap READ getTileMap NOTIFY tilemapReady)
  Q_PROPERTY(Character* player READ getPlayer NOTIFY playerChanged)
  Q_PROPERTY(SoundManager* sounds READ getSoundManager)
  Q_PROPERTY(TaskRunner* tasks MEMBER taskRunner)
  Q_PROPERTY(QQmlListProperty<DynamicObject> dynamicObjects READ getQmlObjects NOTIFY objectsChanged)
  Q_PROPERTY(QQmlListProperty<Character>     visibleCharacters READ getQmlVisibleCharacters NOTIFY visibleCharactersChanged)
public:
  explicit LevelTask(QObject *parent = nullptr);
  virtual ~LevelTask();

  void load(const QString& levelName, DataEngine*);
  void loadObjectsFromDataEngine(DataEngine*);
  void save(DataEngine*);
  void setPaused(bool value) { paused = value; emit pausedChanged(); }

  const QString& getName() const { return name; }
  TileMap* getTileMap() const { return tilemap; }
  SoundManager* getSoundManager() const { return soundManager; }
  Character* getPlayer();
  QList<Character*> findCharacters(std::function<bool (Character&)> compare);

  Q_INVOKABLE bool insertPartyIntoZone(CharacterParty*, const QString& zoneName = "");
  Q_INVOKABLE void tileClicked(int x, int y);

  void registerDynamicObject(DynamicObject*);
  void unregisterDynamicObject(DynamicObject*);

  Q_INVOKABLE QPoint getRenderPositionForTile(int x, int y);
  Q_INVOKABLE DynamicObject* getObjectByName(const QString&);

  Q_INVOKABLE Character* generateCharacter(const QString& name, const QString& characterSheet);
  Q_INVOKABLE StorageObject* generateStorageObject(const QString& name);
  Q_INVOKABLE Doorway* generateDoorway(const QString& name);
  Q_INVOKABLE InventoryItem* generateInventoryItem(const QString& name, const QString& type, int quantity = 1);
  Q_INVOKABLE void deleteObject(DynamicObject* o) { unregisterDynamicObject(o); o->deleteLater(); }

  Q_INVOKABLE void advanceTime(unsigned int minutes);

  QQmlListProperty<DynamicObject> getQmlObjects() { return QQmlListProperty<DynamicObject>(this, &objects); }
  QQmlListProperty<Character> getQmlVisibleCharacters();

  void finalizeRound() override;

  Q_INVOKABLE bool     hasVariable(const QString& name) const { return dataStore.contains(name); }
  Q_INVOKABLE QVariant getVariable(const QString& name) const { return dataStore[name].toVariant(); }
  Q_INVOKABLE void     setVariable(const QString& name, const QVariant& value) { dataStore.insert(name, QJsonValue::fromVariant(value)); }
  Q_INVOKABLE void     unsetVariable(const QString& name) { dataStore.remove(name); }

signals:
  void updated();
  void pausedChanged();
  void objectsChanged();
  void tilemapReady();
  void displayConsoleMessage(const QString&);
  void cameraFocusRequired(DynamicObject*);
  void clickedOnCase(int x, int y);
  void clickedOnObject(DynamicObject*);
  void playerChanged();
  void visibleCharactersChanged();

public slots:
  void deleteLater();

private slots:
  void update();
  void onPauseChanged();
  void onCombatChanged();
  void onCharacterKill(Character* victim, Character* killer);
  void onItemDropped(InventoryItem* item, QPoint position);

  void registerControlZone(TileZone*);
  void unregisterControlZone(TileZone*);

private:
  void loadTilemap(const QString& levelName);

  QTimer            updateTimer;
  QElapsedTimer     clock;
  QString           name;
  TimeManager*      timeManager = nullptr;
  SoundManager*     soundManager = nullptr;
  ScriptController* script = nullptr;
  TaskRunner*       taskRunner = nullptr;
  bool              paused = true;
  QJsonObject       dataStore;
};

#endif // LEVELTASK_H
