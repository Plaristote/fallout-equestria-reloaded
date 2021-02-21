#ifndef  ACTIONQUEUE_H
# define ACTIONQUEUE_H

# include "game/dynamicobject.h"
# include "game/character.h"
# include <QVector>

class ActionQueue : public QObject
{
  Q_OBJECT

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

  class MovementAction : public ActionBase
  {
  public:
    MovementAction(Character* character, QPoint target) : ActionBase(character), target(target)
    {
    }

    int  getApCost() const;
    bool trigger();
    bool isOver();

  protected:
    QPoint target;
  };

  class InteractionAction : public ActionBase
  {
  public:
    InteractionAction(Character* character, DynamicObject* target, QString actionName) : ActionBase(character), target(target), actionName(actionName)
    {
    }

    int  getApCost() const;
    bool trigger();

  protected:
    DynamicObject* target;
    QString        actionName;
  };

  class ItemAction : public ActionBase
  {
  public:
    ItemAction(Character* character, DynamicObject* target, QString itemSlot) : ActionBase(character), target(target), itemSlot(itemSlot)
    {
    }

    int  getApCost() const;
    bool trigger();

  protected:
    DynamicObject* target;
    QString        itemSlot;
  };

public:
  explicit ActionQueue(QObject *parent = nullptr);
  ~ActionQueue();

  void update();
  void reset();

  Q_INVOKABLE int getInteractionApCost(DynamicObject*, const QString& interactionName) const;
  Q_INVOKABLE int getItemUseApCost(DynamicObject* target, const QString& itemSlot) const;
  Q_INVOKABLE int getMovementCost(QPoint target) const;
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
