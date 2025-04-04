#ifndef  BASE_ACTION_H
# define BASE_ACTION_H

# include "game/character.h"

class ActionBase
{
public:
  enum State
  {
    Pending,
    InProgress,
    Done,
    Interrupted
  };

  ActionBase(Character* character) : character(character) {}
  virtual ~ActionBase() {}

  virtual int  getApCost() const = 0;
  virtual void update() {}
  virtual bool trigger() = 0;
  virtual bool canInterrupt() const { return true; }
  virtual void interrupt() {}
  virtual void canceled() {}
  State        getState() const { return state; }

protected:
  Character* character;
  State      state = Pending;
};

#endif
