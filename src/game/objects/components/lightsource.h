#ifndef  LIGHTSOURCECOMPONENT_H
# define LIGHTSOURCECOMPONENT_H

# include "detectable.h"

class TileLayer;

class LightSourceComponent : public DetectableComponent
{
  Q_OBJECT
  typedef DetectableComponent ParentType;

  Q_PROPERTY(int lightRadius MEMBER lightRadius NOTIFY lightRadiusChanged)
  Q_PROPERTY(QColor lightColor READ getLightColor WRITE setLightColor NOTIFY lightColorChanged)
public:
  explicit LightSourceComponent(QObject *parent = nullptr);

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  TileLayer* getLightZone() const { return lightZone; }

  QColor getLightColor() const;
  void setLightColor(QColor);

signals:
  void lightRadiusChanged();
  void lightColorChanged();
  void lightZoneRemoved(TileLayer*);
  void lightZoneAdded(TileLayer*);

public slots:
  void reloadLightzone();
  void refreshLightzone();

private:
  void onFloorChanged();
  void drawLightzone();

  int lightRadius = 0;
  TileLayer* lightZone = nullptr;
  unsigned char currentLightFloor = 0;
};

#endif // LIGHTSOURCECOMPONENT_H
