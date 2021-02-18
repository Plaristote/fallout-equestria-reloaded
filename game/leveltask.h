#ifndef  LEVELTASK_H
# define LEVELTASK_H

# include <QObject>
# include <QElapsedTimer>
# include <QQmlListProperty>
# include "tilemap/tilemap.h"
# include "levelgrid.h"
# include "timermanager.h"

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
  Q_PROPERTY(Character* player READ getPlayer)
  Q_PROPERTY(int        mouseMode MEMBER mouseMode NOTIFY mouseModeChanged)
  Q_PROPERTY(QQmlListProperty<DynamicObject> dynamicObjects READ getQmlObjects NOTIFY objectsChanged)
public:
  enum MouseMode {
    MovementCursor    = 0,
    InteractionCursor = 1,
    TargetCursor      = 2
  };

  explicit LevelTask(QObject *parent = nullptr);

  void load(const QString& levelName, DataEngine*);
  void loadObjectsFromTilemap();
  void loadObjectsFromDataEngine(DataEngine*);
  void save(DataEngine*);
  void setPaused(bool value) { paused = value; emit pausedChanged(); }

  const QString& getName() const { return name; }
  TileMap* getTileMap() const { return tilemap; }
  Character* getPlayer();
  QList<Character*> findCharacters(std::function<bool (Character&)> compare);

  Q_INVOKABLE bool insertPartyIntoZone(CharacterParty*, const QString& zoneName = "");
  Q_INVOKABLE void tileClicked(int x, int y);
  Q_INVOKABLE void objectClicked(DynamicObject*);
  Q_INVOKABLE void swapMouseMode();

  void registerDynamicObject(DynamicObject*);
  void unregisterDynamicObject(DynamicObject*);

  Q_INVOKABLE QPoint getRenderPositionForTile(int x, int y);
  Q_INVOKABLE DynamicObject* getObjectByName(const QString&);

  Q_INVOKABLE Character* generateCharacter(const QString& name, const QString& characterSheet);
  Q_INVOKABLE StorageObject* generateStorageObject(const QString& name);

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
  void mouseModeChanged();

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
  bool          paused = true;
  int           mouseMode = 0;
};

#endif // LEVELTASK_H
