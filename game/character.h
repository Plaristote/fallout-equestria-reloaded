#ifndef  CHARACTER_H
# define CHARACTER_H

# include "globals.h"
# include "cmap/statmodel.h"
# include "diplomacy.hpp"
# include "characters/field_of_view.hpp"
# include "characters/buffs.h"
# define CHARACTER_MAX_MORALE 100

class ActionQueue;

class Character : public CharacterBuffs
{
  Q_OBJECT
  typedef CharacterBuffs ParentType;

  Q_PROPERTY(ActionQueue* actionQueue READ getActionQueue CONSTANT)
  Q_PROPERTY(int actionPoints MEMBER actionPoints NOTIFY actionPointsChanged)
  Q_PROPERTY(bool unconscious READ isUnconscious NOTIFY unconsciousChanged)
  Q_PROPERTY(int morale MEMBER morale NOTIFY moraleChanged)
public:
  explicit Character(QObject *parent = nullptr);

  void update(qint64) override;
  void load(const QJsonObject&) override;
  void save(QJsonObject&) const override;

  ActionQueue* getActionQueue() const { return actionQueue; }
  Q_INVOKABLE QString getDialogName();
  unsigned int getXpValue() const;
  int              getMorale() const { return morale; }
  bool             shouldJoinFight() const;
  Q_INVOKABLE bool isMoving() const { return Sprite::isMoving() || currentPath.size() > 0; }
  bool         isSpriteMoving() const { return Sprite::isMoving(); }
  int          getZIndex() const override { return isAlive() ? 2 : 1; }
  int          getCoverValue() const override { return 10; }
  Q_INVOKABLE void moveAway(Character*);

  int getInteractionDistance() const override;
  int  getSneakAbility() const override;
  bool isBlockingPath() const override { return isAlive(); }

  Q_INVOKABLE QJSValue getActions();
  bool             isUnconscious() const override { return unconscious; }
  bool             hasInteractionOverlay() const override { return !isAlive(); }

  Q_INVOKABLE void  takeDamage(int damage, Character* dealer);
  Q_INVOKABLE void  attackedBy(Character* dealer);
  Q_INVOKABLE bool  useActionPoints(int amount = 1, const QString& motive = "");
  inline int        getActionPoints() const { return actionPoints; }
  void              resetActionPoints();
  Q_INVOKABLE void  fallAwayFrom(int x, int y, int distance);
  Q_INVOKABLE void  fall(int distance, const QString& direction);
  void              fall(int distance, Direction direction);
  Q_INVOKABLE void  fallUnconscious();
  Q_INVOKABLE void  wakeUp();
  bool              setFallAnimation();

signals:
  void actionPointsChanged();
  void requireJoinCombat();
  void died();
  void characterKill(Character* victim, Character* killer);
  void unconsciousChanged();
  void moraleChanged();

private slots:
  void onActionQueueCompleted();
  void afterDeathAnimation();

private:
  QVector<QPoint> getAvailableSurroundingCases() const;
  virtual QString getScriptPath() const override { return SCRIPTS_PATH + "pnjs"; }

  ActionQueue* actionQueue;
  bool unconscious = false;
  int actionPoints = 0;
  int morale = CHARACTER_MAX_MORALE;
  QJSValue jsActionQueue;
};

#endif // CHARACTER_H
