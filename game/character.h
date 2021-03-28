#ifndef  CHARACTER_H
# define CHARACTER_H

# include "globals.h"
# include "cmap/statmodel.h"
# include "diplomacy.hpp"
# include "characters/diplomacy.h"
# include "characters/field_of_view.hpp"

class ActionQueue;

class Character : public CharacterDiplomacy
{
  Q_OBJECT
  typedef CharacterDiplomacy ParentType;

  Q_PROPERTY(FieldOfView* fieldOfView MEMBER fieldOfView)
  Q_PROPERTY(ActionQueue* actionQueue MEMBER actionQueue)
  Q_PROPERTY(int actionPoints MEMBER actionPoints NOTIFY actionPointsChanged)
public:
  explicit Character(QObject *parent = nullptr);

  void update(qint64) override;
  void load(const QJsonObject&) override;
  void save(QJsonObject&) const override;
  void setScript(const QString&) override;

  FieldOfView* getFieldOfView() const { return fieldOfView; }
  ActionQueue* getActionQueue() const { return actionQueue; }
  QString      getDialogName();
  unsigned int getXpValue() const;
  Q_INVOKABLE bool isMoving() const { return Sprite::isMoving() || currentPath.size() > 0; }
  bool         isSpriteMoving() const { return Sprite::isMoving(); }
  int          getZIndex() const override { return isAlive() ? 2 : 1; }
  int          getCoverValue() const override { return 10; }

  Q_INVOKABLE QPoint getInteractionPosition() const override;
  bool isBlockingPath() const override { return isAlive(); }

  Q_INVOKABLE QJSValue getActions();
  Q_INVOKABLE bool isAlive() const { return getStatistics()->getHitPoints() > 0; }
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

private slots:
  void initializeEmptySlots();
  void initializeEmptySlot(const QString& name);
  void onActionQueueCompleted();

private:
  virtual QString getScriptPath() const override { return SCRIPTS_PATH + "pnjs"; }
  QString getDefaultItemForSlot(const QString& name);

  FieldOfView* fieldOfView;
  ActionQueue* actionQueue;
  bool sneakEnabled = false;
  int actionPoints = 0;
  QJSValue jsActionQueue;
};

#endif // CHARACTER_H
