#pragma once
#include "zone.h"
#include <QSet>

class TileLayer;

class LightingComponent : public ZoneComponent
{
  Q_OBJECT
  typedef ZoneComponent ParentType;
public:
  explicit LightingComponent(QObject *parent = nullptr);

  Q_INVOKABLE bool isLit(int x, int y) const { return isLit(QPoint(x, y)); }
  bool isLit(QPoint position) const;
  const QList<TileLayer*> &activeLights() const { return m_activeLights; }

signals:
  void activeLightsChanged();
  void lightLayerTilesChanged(TileLayer *layer);

private slots:
  void onFloorChanged();
  void onLightsChanged();
  void onLightVisibilityChanged();
  void onLightTilesChanged();

private:
  void resubscribeToLights();
  void rebuildLitTiles();

  static qint64 pack(int x, int y)
  {
    return (static_cast<qint64>(x) << 32) | static_cast<quint32>(y);
  }

  QSet<qint64> m_litTiles;
  QList<TileLayer *> m_activeLights;
  QVector<QMetaObject::Connection> m_lightConnections;
};
