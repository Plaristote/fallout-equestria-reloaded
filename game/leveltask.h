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
  Q_PROPERTY(QQmlListProperty<DynamicObject> dynamicObjects READ getQmlObjects NOTIFY objectsChanged)
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
  Q_INVOKABLE void deleteObject(DynamicObject* o) { unregisterDynamicObject(o); }

  QQmlListProperty<DynamicObject> getQmlObjects() { return QQmlListProperty<DynamicObject>(this, &objects); }

signals:
  void updated();
  void pausedChanged();
  void objectsChanged();
  void tilemapReady();
  void displayConsoleMessage(const QString&);
  void exitZoneEntered(TileZone*);
  void cameraFocusRequired(DynamicObject*);
  void clickedOnCase(int x, int y);
  void clickedOnObject(DynamicObject*);
  void playerChanged();

public slots:
  void deleteLater();

private slots:
  void update();
  void onPauseChanged();

  void onZoneEntered(DynamicObject*, TileZone*);
  void onZoneExited(DynamicObject*, TileZone*);
  void registerControlZone(TileZone*);
  void unregisterControlZone(TileZone*);

private:
  QList<DynamicObject*> objects;

  QTimer        updateTimer;
  QElapsedTimer clock;
  QString       name;
  TileMap*      tilemap = nullptr;
  TimeManager*  timeManager = nullptr;
  SoundManager* soundManager = nullptr;
  bool          paused = true;
};

#endif // LEVELTASK_H
