#ifndef  BASE_ACTION_H
# define BASE_ACTION_H

# include "game/character.h"

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

#endif
