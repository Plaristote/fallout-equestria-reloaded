#ifndef  LEVELTASK_H
# define LEVELTASK_H

# include <QObject>
# include <QElapsedTimer>
# include <QQmlListProperty>
# include "tilemap/tilemap.h"
# include "timermanager.h"
# include "soundmanager.h"

# include "character.h"
# include "characterparty.h"
# include "dynamicobject.h"

# include "level/save.h"
# include "level/tutorialcomponent.h"
# include "level/metrics.h"

class Doorway;
class CharacterParty;
class CharacterDialog;
class DataEngine;

class LevelTask : public SaveComponent
{
  Q_OBJECT
  typedef SaveComponent ParentType;

  Q_PROPERTY(bool       paused  MEMBER paused NOTIFY pausedChanged)
  Q_PROPERTY(TutorialComponent* tutorial MEMBER tutorial NOTIFY tutorialChanged)
public:  
  static bool withPlayerCropCircle;

  explicit LevelTask(QObject *parent = nullptr);
  virtual ~LevelTask() override;

  void setPaused(bool value) { paused = value; emit pausedChanged(); }
  bool isPaused() const { return paused; }

  const QString& getName() const { return name; }

  void registerDynamicObject(DynamicObject*) override;
  void unregisterDynamicObject(DynamicObject*) override;

  Q_INVOKABLE CharacterParty* createNpcGroup(QJSValue description) { return CharacterParty::factory(description.toVariant().toMap(), this); }
  Q_INVOKABLE void addBloodStainAt(int x, int y, unsigned char z) { addBloodStainAt(QPoint(x, y), z); }
  void             addBloodStainAt(QPoint, unsigned char floor);

  void onExit();
  void finalizeRound() override;

signals:
  void updated();
  void pausedChanged();
  void displayConsoleMessage(const QString&);
  void cameraFocusRequired(DynamicObject*);
  void daylightColorChanged();

public slots:
  void deleteLater();

private slots:
  virtual void update();
  void onPauseChanged();
  void onCharacterKill(Character* victim, Character* killer);
  void onItemDropped(InventoryItem* item, QPoint position, unsigned char floor = NULL_FLOOR);
  void displayMovementTargetHint(QPoint position);

private:
  void onCombatStateChanged() override;
  void updateRoofVisibility();
  void combatTask(qint64);
  void endTurnTask(qint64);
  void realTimeTask(qint64);

protected:
  QTimer         updateTimer;
  QElapsedTimer  clock;
  bool           paused = true;
  bool           initialized = false;
  qint64         finalizeTurnRemainingTime = 0;
};

#endif // LEVELTASK_H
