#ifndef  RACE_H
# define RACE_H

# include "cmap/plugin.h"
# include "game/animationlibrary.h"

class Race : public CmapPlugin
{
public:
  bool    isPlayable() const;
  CharacterSpriteDescriptor getSpriteSheet(StatModel*) const;

  static void initialize();
  static const QMap<QString, Race>& getRaces() { return races; }

private:
  static QMap<QString, Race> races;
};

#endif // RACE_H
