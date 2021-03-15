#ifndef  RACE_H
# define RACE_H

# include "cmap/plugin.h"

class Race : public CmapPlugin
{
public:
  bool    isPlayable() const;
  QString getSpriteSheet(StatModel*) const;
};

#endif // RACE_H
