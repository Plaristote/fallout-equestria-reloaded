#ifndef  MY_ACTIONQUEUE_H
# define MY_ACTIONQUEUE_H

# include "game/dynamicobject.h"
# include "game/character.h"
# include <QVector>

class ActionBase
{
public:
  ActionBase(Character* character) : character(character) {}
  virtual ~ActionBase() {}

  virtual int  getApCost() const = 0;
  virtual bool trigger() = 0;
  virtual bool isOver() { return true; }

protected:
  Character* character;
};

class ActionQueue : public QObject
{
  Q_OBJECT
public:
  ActionQueue(QObject *parent = nullptr);
  ~ActionQueue();

  void update();
  void reset();

  Q_INVOKABLE int getInteractionApCost(DynamicObject*, const QString& interactionName) const;
  Q_INVOKABLE int getItemUseApCost(DynamicObject* target, const QString& itemSlot) const;
  Q_INVOKABLE int getMovementApCost(QPoint target) const;
  Q_INVOKABLE void pushInteraction(DynamicObject* target, const QString& interactionName);
  Q_INVOKABLE void pushItemUse(DynamicObject* target, const QString& itemSlot);
  Q_INVOKABLE void pushMovement(QPoint target);
  Q_INVOKABLE bool start();

signals:
  void queueCompleted();

private:
  void onActionOver();

  Character* character;
  QVector<ActionBase*> queue;
};

#endif // ACTIONQUEUE_H
