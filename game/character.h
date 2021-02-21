#ifndef  CHARACTER_H
# define CHARACTER_H

# include "globals.h"
# include "storageobject.h"
# include "cmap/statmodel.h"
# include "diplomacy.hpp"
# include "characters/field_of_view.hpp"

class ActionQueue;

class Character : public StorageObject
{
  Q_OBJECT

  Q_PROPERTY(StatModel*   statistics  MEMBER statistics)
  Q_PROPERTY(FieldOfView* fieldOfView MEMBER fieldOfView)
  Q_PROPERTY(ActionQueue* actionQueue MEMBER actionQueue)
  Q_PROPERTY(int actionPoints MEMBER actionPoints NOTIFY actionPointsChanged)
public:
  explicit Character(QObject *parent = nullptr);

  void update(qint64) override;
  void load(const QJsonObject&) override;
  void save(QJsonObject&) const override;
  void setScript(const QString&) override;

  void         setStatistics(StatModel* value) { statistics = value; }
  StatModel*   getStatistics() const { return statistics; }
  FieldOfView* getFieldOfView() const { return fieldOfView; }
  ActionQueue* getActionQueue() const { return actionQueue; }
  QString      getFactionName() const { return faction ? faction->name : QString(); }
  unsigned int getFactionFlag() const { return faction ? faction->flag : 0; }
  QString      getDialogName();
  bool         getIsUnique() const { return isUnique; }
  void         setUnique(bool value) { isUnique = value; }
  bool         isMoving() const { return Sprite::isMoving() || currentPath.size() > 0; }

  // TODO ger rid o that
  Q_INVOKABLE int a_getInteractionApCost(DynamicObject*, const QString& interactionName) const;
  Q_INVOKABLE int a_getItemUseApCost(DynamicObject* target, const QString& itemSlot) const;
  int a__getMovementApCost(QPoint target) const;
  Q_INVOKABLE int a_getMovementApCost(int x, int y) const { return a__getMovementApCost(QPoint(x, y)); }
  Q_INVOKABLE void a_pushInteraction(DynamicObject* target, const QString& interactionName);
  Q_INVOKABLE void a_pushItemUse(DynamicObject* target, const QString& itemSlot);
  void a__pushMovement(QPoint target);
  Q_INVOKABLE void a_pushMovement(int x, int y) { return a__pushMovement(QPoint(x, y)); }
  Q_INVOKABLE void a_reset();
  Q_INVOKABLE bool a_start();
  // END TODO

  Q_INVOKABLE bool renderOnTile() const { return true; }
  Q_INVOKABLE QPoint getInteractionPosition() const override;
  bool isBlockingPath() const override { return isAlive(); }

  Q_INVOKABLE bool isAlive() const { return getStatistics()->getHitPoints() > 0; }
  Q_INVOKABLE bool isAlly(const Character*) const;
  Q_INVOKABLE bool isEnemy(const Character*) const;
  Q_INVOKABLE void setAsEnemy(Character*);
  Q_INVOKABLE bool hasLineOfSight(const Character*) const;
  Q_INVOKABLE bool isSneaking() const { return sneakEnabled; }
  Q_INVOKABLE float getDistance(const DynamicObject*) const;

  Q_INVOKABLE void takeDamage(int damage, Character* dealer);
  Q_INVOKABLE bool useActionPoints(int amount = 1, const QString& motive = "");
  inline int       getActionPoints() const { return actionPoints; }
  void             resetActionPoints();

  Q_INVOKABLE void updateInventorySlots();

signals:
  void actionPointsChanged();
  void requireJoinCombat();
  void died();

protected:
  virtual QString getScriptPath() const override { return SCRIPTS_PATH + "pnjs"; }

private slots:
  void initializeFaction();
  void initializeEmptySlots();
  void initializeEmptySlot(const QString& name);
  void onActionQueueCompleted();

private:
  QString getDefaultItemForSlot(const QString& name);

  StatModel* statistics = nullptr;
  FieldOfView* fieldOfView;
  ActionQueue* actionQueue;
  WorldDiplomacy::Faction* faction = nullptr;
  bool isUnique = false;
  bool sneakEnabled = false;
  unsigned int enemyFlag = 0;
  int actionPoints = 0;
};

#endif // CHARACTER_H
