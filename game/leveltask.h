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
# include "level/tutorialcomponent.h"

class Doorway;
class CharacterParty;
class CharacterDialog;
class DataEngine;

class LevelTask : public CombatComponent
{
  Q_OBJECT
  typedef CombatComponent ParentType;

  Q_PROPERTY(bool       paused  MEMBER paused NOTIFY pausedChanged)
  Q_PROPERTY(SoundManager* sounds READ getSoundManager CONSTANT)
  Q_PROPERTY(TutorialComponent* tutorial MEMBER tutorial NOTIFY tutorialChanged)
  Q_PROPERTY(bool debugMode MEMBER debugMode NOTIFY debugModeChanged)
  Q_PROPERTY(bool persistent MEMBER persistent NOTIFY persistentChanged)
public:  
  explicit LevelTask(QObject *parent = nullptr);
  virtual ~LevelTask();

  void load(const QString& levelName, DataEngine*);
  void passElapsedTime(int lastUpdate);
  void loadObjectsFromDataEngine(DataEngine*);
  void loadTutorial();
  void save(DataEngine*);
  void setPaused(bool value) { paused = value; emit pausedChanged(); }
  bool isPaused() const { return paused; }

  const QString& getName() const { return name; }
  SoundManager* getSoundManager() const { return soundManager; }

  Q_INVOKABLE bool insertPartyIntoZone(CharacterParty*, const QString& zoneName = "");
  Q_INVOKABLE void tileClicked(int x, int y);

  void registerDynamicObject(DynamicObject*) override;
  void unregisterDynamicObject(DynamicObject*) override;

  Q_INVOKABLE void persist();

  Q_INVOKABLE void advanceTime(unsigned int minutes);

  Q_INVOKABLE QVariantList previewPathTo(int x, int y);

  Q_INVOKABLE void addBloodStainAt(int x, int y, unsigned char z) { addBloodStainAt(QPoint(x, y), z); }
  void             addBloodStainAt(QPoint, unsigned char floor);

  void finalizeRound() override;

signals:
  void updated();
  void pausedChanged();
  void debugModeChanged();
  void objectsChanged();
  void tutorialChanged();
  void persistentChanged();
  void displayConsoleMessage(const QString&);
  void cameraFocusRequired(DynamicObject*);
  void visibleCharactersChanged();
  void clickedOnCase(int x, int y);
  void clickedOnObject(DynamicObject*);

public slots:
  void deleteLater();

private slots:
  virtual void update();
  void onPauseChanged();
  void onCombatChanged();
  void onCharacterKill(Character* victim, Character* killer);
  void onItemDropped(InventoryItem* item, QPoint position, unsigned char floor = NULL_FLOOR);
  void displayMovementTargetHint(QPoint position);

private:
  void combatTask(qint64);
  void realTimeTask(qint64);

protected:
  QTimer             updateTimer;
  QElapsedTimer      clock;
  TimeManager*       timeManager = nullptr;
  SoundManager*      soundManager = nullptr;
  TutorialComponent* tutorial = nullptr;
  bool               paused = true;
  bool               initialized = false;
  bool               persistent = true;
  bool               debugMode = false;
};

#endif // LEVELTASK_H
