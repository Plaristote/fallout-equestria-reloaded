#ifndef  RACE_H
# define RACE_H

# include "cmap/plugin.h"
# include "game/animationlibrary.h"

class Race : public CmapPlugin
{
public:
  struct AgeRange
  {
    unsigned short child = 0, adult = 15, old = 60, lifespan = 120;
    QVariantMap toVariant() const;
  };

  bool                      isPlayable() const;
  bool                      withFaceColor() const;
  AgeRange                  getAgeRange() const;
  QStringList               getFaces() const;
  QStringList               getHairs(QString face = "") const;
  CharacterSpriteDescriptor getSpriteSheet(StatModel*) const;
  QMap<QString, QString>    getItemSlots() const;
  QString                   getDefaultItemForSlot(StatModel*, const QString&) const;

  static void initialize();
  static const QMap<QString, Race>& getRaces() { return races; }

private:
  static QMap<QString, Race> races;
};

#endif // RACE_H
