#ifndef  MY_ACTIONQUEUE_H
# define MY_ACTIONQUEUE_H

# include "game/dynamicobject.h"
# include <QVector>

class ActionBase;

class ActionQueue : public QObject
{
  Q_OBJECT
public:
  ActionQueue(QObject *parent = nullptr);
  ~ActionQueue();

  void update();
  void pause();
  void unpause();
  Q_INVOKABLE void reset();
  Q_INVOKABLE inline bool isEmpty() { return queue.empty(); }

  Q_INVOKABLE int getInteractionApCost(DynamicObject*, const QString& interactionName) const;
  Q_INVOKABLE int getItemUseApCost(DynamicObject* target, const QString& itemSlot) const;
  Q_INVOKABLE int getSkillUseApCost(DynamicObject* target, const QString& skillName) const;
  int             getMovementApCost(QPoint target) const;
  Q_INVOKABLE int getMovementApCost(int x, int y) const { return getMovementApCost(QPoint(x, y)); }
  Q_INVOKABLE int getReachApCost(DynamicObject* target, float range) const;
  Q_INVOKABLE void pushInteraction(DynamicObject* target, const QString& interactionName);
  Q_INVOKABLE void pushItemUse(DynamicObject* target, const QString& itemSlot);
  Q_INVOKABLE void pushSkillUse(DynamicObject* target, const QString& name);
  void             pushMovement(QPoint target);
  Q_INVOKABLE void pushMovement(int x, int y) { pushMovement(QPoint(x, y)); }
  Q_INVOKABLE void pushReach(DynamicObject* target, float range = 1);
  Q_INVOKABLE bool start();

signals:
  void queueCompleted();

private:
  void onActionOver();

  Character* character;
  QVector<ActionBase*> queue;
};

#endif // ACTIONQUEUE_H
