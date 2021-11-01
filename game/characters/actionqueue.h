#ifndef  MY_ACTIONQUEUE_H
# define MY_ACTIONQUEUE_H

# include "game/dynamicobject.h"
# include <QVector>

class ActionBase;
class InventoryItem;

class ActionQueue : public QObject
{
  Q_OBJECT
public:
  ActionQueue(QObject *parent = nullptr);
  ~ActionQueue();

  void update();
  void pause();
  void unpause();
  bool canInterrupt() const;
  Q_INVOKABLE void reset();
  Q_INVOKABLE inline bool isEmpty() { return queue.empty(); }

  Q_INVOKABLE int getInteractionApCost(DynamicObject*, const QString& interactionName) const;
  Q_INVOKABLE int getItemUseApCost(DynamicObject* target, const QString& itemSlot) const;
  Q_INVOKABLE int getSkillUseApCost(DynamicObject* target, const QString& skillName) const;
  int             getMovementApCost(QPoint target) const;
  Q_INVOKABLE int getMovementApCost(int x, int y) const { return getMovementApCost(QPoint(x, y)); }
  Q_INVOKABLE int getReachApCost(DynamicObject* target, float range = 1) const;
  Q_INVOKABLE int getReachApCost(DynamicObject* target, float range, QJSValue caseCompare);
  Q_INVOKABLE int getReachCaseApCost(int x, int y, float range = 1) const;
  Q_INVOKABLE int getReachCaseApCost(int x, int y, float range, QJSValue caseCompare);
  Q_INVOKABLE void pushInteraction(DynamicObject* target, const QString& interactionName);
  Q_INVOKABLE void pushItemUse(DynamicObject* target, const QString& itemSlot);
  Q_INVOKABLE void pushItemUse(DynamicObject* target, InventoryItem* item);
  Q_INVOKABLE void pushItemUseAt(int x, int y, const QString& itemSlot);
  Q_INVOKABLE void pushItemUseAt(int x, int y, InventoryItem* item);
  Q_INVOKABLE void pushSkillUse(DynamicObject* target, const QString& name);
  void             pushMovement(QPoint target);
  Q_INVOKABLE void pushMovement(int x, int y) { pushMovement(QPoint(x, y)); }
  Q_INVOKABLE void pushReach(DynamicObject* target, float range = 1);
  Q_INVOKABLE void pushReach(DynamicObject* target, float range, QJSValue caseCompare);
  Q_INVOKABLE void pushReachCase(int x, int y, float range);
  Q_INVOKABLE void pushReachCase(int x, int y, float range, QJSValue caseCompare);
  Q_INVOKABLE void pushReachNear(int x, int y, int range);
  Q_INVOKABLE void pushReachCase(int x, int y, int z, float range);
  Q_INVOKABLE void pushReachCase(int x, int y, int z, float range, QJSValue caseCompare);
  Q_INVOKABLE void pushReachNear(int x, int y, int z, int range);
  Q_INVOKABLE void pushWait(unsigned int seconds);
  Q_INVOKABLE void pushSpeak(const QString& content, unsigned int interval, const QString& color);
  Q_INVOKABLE void pushLookAt(const DynamicObject* target);
  Q_INVOKABLE void pushLookAt(int x, int y);
  void             pushSliding(QPoint target);
  Q_INVOKABLE bool start();

signals:
  void queueCompleted();

private:
  void clearStack();
  void clearStash();
  void runQueue();
  void onActionOver();

  Character* character;
  QVector<ActionBase*> queue;
  QVector<ActionBase*> stash;
  bool resetFlag = false;
};

#endif // ACTIONQUEUE_H
