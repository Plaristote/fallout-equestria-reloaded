#ifndef  TILED_PROPERTIES_H
# define TILED_PROPERTIES_H

# include <QVariant>
# include <QVariantMap>

class QJsonObject;

enum TiledPropertyVersion
{
  TiledProperty_1_8,
  TiledProperty_1_9,
  TiledProperty_1_10
};

TiledPropertyVersion getTiledPropertyVersion(const QString& tiledVersion);
QVariantMap loadTiledProperties(const QJsonObject&, const QString& tiledVersion);
QVariantMap loadTiledProperties(const QJsonObject&, TiledPropertyVersion = TiledProperty_1_10);

#endif
