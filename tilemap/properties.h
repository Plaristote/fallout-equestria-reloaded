#ifndef  TILED_PROPERTIES_H
# define TILED_PROPERTIES_H

# include <QVariant>
# include <QVariantMap>

class QJsonObject;

QVariantMap loadTiledProperties(const QJsonObject&, const QString& tiledVersion = "1.9");

#endif
