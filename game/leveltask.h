#ifndef  LEVELTASK_H
# define LEVELTASK_H

# include <QObject>
# include <QElapsedTimer>
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
public:
  explicit LevelTask(QObject *parent = nullptr);

  void load(const QString& levelName, DataEngine*);
  void loadObjectsFromTilemap();
  void loadObjectsFromDataEngine(DataEngine*);
  void save(DataEngine*);
  void setPaused(bool value) { paused = value; emit pausedChanged(); }

  const QString& getName() const { return name; }
  TileMap* getTileMap() const { return tilemap; }
  Character* getPlayer();

  Q_INVOKABLE bool insertPartyIntoZone(CharacterParty*, const QString& zoneName = "");
  Q_INVOKABLE void tileClicked(int x, int y);

  void registerDynamicObject(DynamicObject*);
  void unregisterDynamicObject(DynamicObject*);

  Q_INVOKABLE QPoint getRenderPositionForTile(int x, int y);

signals:
  void updated();
  void pausedChanged();
  void tilemapReady();
  void displayConsoleMessage(const QString&);
  void exitZoneEntered(TileZone*);
  void cameraFocusRequired(DynamicObject*);

private slots:
  void update();
  void onPauseChanged();

  void onZoneEntered(DynamicObject*, TileZone*);
  void onZoneExited(DynamicObject*, TileZone*);

private:
  QList<DynamicObject*> objects;

  QTimer        updateTimer;
  QElapsedTimer clock;
  QString       name;
  TileMap*      tilemap = nullptr;
  TimeManager*  timeManager = nullptr;
  bool          paused = true;
};

#endif // LEVELTASK_H
