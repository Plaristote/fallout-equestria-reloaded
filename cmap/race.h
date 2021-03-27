#ifndef  RACE_H
# define RACE_H

# include "cmap/plugin.h"

class Race : public CmapPlugin
{
public:
  bool    isPlayable() const;
  QString getSpriteSheet(StatModel*) const;

  static void initialize();
  static const QMap<QString, Race>& getRaces() { return races; }

private:
  static QMap<QString, Race> races;
};

#endif // RACE_H
