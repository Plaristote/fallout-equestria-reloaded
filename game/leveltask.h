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
  typedef CombatComponent ParentType;

  Q_PROPERTY(bool       paused  MEMBER paused NOTIFY pausedChanged)
  Q_PROPERTY(SoundManager* sounds READ getSoundManager)
  Q_PROPERTY(TaskRunner* tasks MEMBER taskRunner)
public:  
  explicit LevelTask(QObject *parent = nullptr);
  virtual ~LevelTask();

  void load(const QString& levelName, DataEngine*);
  void loadObjectsFromDataEngine(DataEngine*);
  void save(DataEngine*);
  void setPaused(bool value) { paused = value; emit pausedChanged(); }

  const QString& getName() const { return name; }
  SoundManager* getSoundManager() const { return soundManager; }

  Q_INVOKABLE bool insertPartyIntoZone(CharacterParty*, const QString& zoneName = "");
  Q_INVOKABLE void tileClicked(int x, int y);

  void registerDynamicObject(DynamicObject*);
  void unregisterDynamicObject(DynamicObject*);

  Q_INVOKABLE Character*     generateCharacter(const QString& name, const QString& characterSheet);
  Q_INVOKABLE StorageObject* generateStorageObject(const QString& name);
  Q_INVOKABLE Doorway*       generateDoorway(const QString& name);
  Q_INVOKABLE InventoryItem* generateInventoryItem(const QString& name, const QString& type, int quantity = 1);
  Q_INVOKABLE DynamicObject* generateDynamicObject(const QString& name);
  Q_INVOKABLE void           deleteObject(DynamicObject* o);

  Q_INVOKABLE void advanceTime(unsigned int minutes);

  Q_INVOKABLE QVariantList previewPathTo(int x, int y);

  Q_INVOKABLE void addBloodStainAt(int x, int y) { addBloodStainAt(QPoint(x, y)); }
  void             addBloodStainAt(QPoint);

  void finalizeRound() override;

signals:
  void updated();
  void pausedChanged();
  void objectsChanged();
  void displayConsoleMessage(const QString&);
  void cameraFocusRequired(DynamicObject*);
  void clickedOnCase(int x, int y);
  void clickedOnObject(DynamicObject*);
  void visibleCharactersChanged();

public slots:
  void deleteLater();

private slots:
  virtual void update();
  void onPauseChanged();
  void onCombatChanged();
  void onCharacterKill(Character* victim, Character* killer);
  void onItemDropped(InventoryItem* item, QPoint position);

private:
  void displayMovementTargetHint(QPoint position);

protected:
  QTimer            updateTimer;
  QElapsedTimer     clock;
  TimeManager*      timeManager = nullptr;
  SoundManager*     soundManager = nullptr;
  TaskRunner*       taskRunner = nullptr;
  bool              paused = true;
  bool              initialized = false;
};

#endif // LEVELTASK_H
