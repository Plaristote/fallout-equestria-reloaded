#ifndef  CHARACTER_H
# define CHARACTER_H

# include "globals.h"
# include "cmap/statmodel.h"
# include "diplomacy.hpp"
# include "characters/movement.h"
# include "characters/field_of_view.hpp"

class ActionQueue;

class Character : public CharacterMovement
{
  Q_OBJECT

  Q_PROPERTY(StatModel*   statistics  MEMBER statistics NOTIFY statisticsChanged)
  Q_PROPERTY(FieldOfView* fieldOfView MEMBER fieldOfView)
  Q_PROPERTY(ActionQueue* actionQueue MEMBER actionQueue)
  Q_PROPERTY(int actionPoints MEMBER actionPoints NOTIFY actionPointsChanged)
  // Editor properties
  Q_PROPERTY(bool isUnique MEMBER isUnique NOTIFY uniqueChanged)
  Q_PROPERTY(QString characterSheet MEMBER characterSheet NOTIFY characterSheetChanged)
public:
  explicit Character(QObject *parent = nullptr);

  void update(qint64) override;
  void load(const QJsonObject&) override;
  void save(QJsonObject&) const override;
  void setScript(const QString&) override;
  void setCharacterSheet(const QString&);

  void         setStatistics(StatModel* value);
  StatModel*   getStatistics() const { return statistics; }
  FieldOfView* getFieldOfView() const { return fieldOfView; }
  ActionQueue* getActionQueue() const { return actionQueue; }
  QString      getFactionName() const { return faction ? faction->name : QString(); }
  unsigned int getFactionFlag() const { return faction ? faction->flag : 0; }
  QString      getDialogName();
  unsigned int getXpValue() const;
  bool         getIsUnique() const { return isUnique; }
  void         setUnique(bool value) { isUnique = value; }
  Q_INVOKABLE bool isMoving() const { return Sprite::isMoving() || currentPath.size() > 0; }
  int          getZIndex() const override { return isAlive() ? 2 : 1; }
  const QVector<TileZone*>& getCurrentZones() const { return currentZones; }

  Q_INVOKABLE QPoint getInteractionPosition() const override;
  bool isBlockingPath() const override { return isAlive(); }

  Q_INVOKABLE QJSValue getActions();
  Q_INVOKABLE bool isAlive() const { return getStatistics()->getHitPoints() > 0; }
  Q_INVOKABLE bool isAlly(const Character*) const;
  Q_INVOKABLE bool isEnemy(const Character*) const;
  Q_INVOKABLE bool isInZone(TileZone* value) const { return currentZones.indexOf(value) >= 0; }
  Q_INVOKABLE void setAsEnemy(Character*);
  Q_INVOKABLE bool hasLineOfSight(const Character*) const;
  Q_INVOKABLE bool isSneaking() const { return sneakEnabled; }
  float             getDistance(const DynamicObject*) const;
  Q_INVOKABLE float getDistance(DynamicObject* v) const;

  Q_INVOKABLE void takeDamage(int damage, Character* dealer);
  Q_INVOKABLE bool useActionPoints(int amount = 1, const QString& motive = "");
  inline int       getActionPoints() const { return actionPoints; }
  void             resetActionPoints();

  Q_INVOKABLE void updateInventorySlots();

signals:
  void actionPointsChanged();
  void requireJoinCombat();
  void died();
  void characterKill(Character* victim, Character* killer);
  // EDITOR signals
  void uniqueChanged();
  void statisticsChanged();
  void characterSheetChanged();

public slots:
  void onZoneEntered(TileZone* value) { currentZones.append(value); }
  void onZoneExited(TileZone* value) { currentZones.removeOne(value); }

private slots:
  void initializeFaction();
  void initializeEmptySlots();
  void initializeEmptySlot(const QString& name);
  void onActionQueueCompleted();
  void onCharacterSheetChanged();

private:
  virtual QString getScriptPath() const override { return SCRIPTS_PATH + "pnjs"; }
  QString getDefaultItemForSlot(const QString& name);

  QString characterSheet;
  StatModel* statistics = nullptr;
  FieldOfView* fieldOfView;
  ActionQueue* actionQueue;
  WorldDiplomacy::Faction* faction = nullptr;
  bool isUnique = false;
  bool sneakEnabled = false;
  unsigned int enemyFlag = 0;
  int actionPoints = 0;
  QVector<TileZone*> currentZones;
};

#endif // CHARACTER_H
