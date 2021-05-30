#ifndef  TILED_PROPERTIES_H
# define TILED_PROPERTIES_H

# include <QVariant>
# include <QVariantMap>

class QJsonObject;

QVariant tiledPropertyToVariant(const QJsonObject&);
QVariantMap loadTiledProperties(const QJsonObject&);

#endif
