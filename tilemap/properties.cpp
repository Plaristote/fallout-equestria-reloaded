#include "properties.h"
#include <QJsonObject>
#include <QJsonArray>

QVariant tiledPropertyToVariant(const QJsonObject& object)
{
  QString typeName = object["type"].toString();

  if (typeName == "bool")
    return object["value"].toBool();
  if (typeName == "string" || typeName == "color")
    return object["value"].toString();
  if (typeName == "float")
    return object["value"].toDouble();
  if (typeName == "int")
    return object["value"].toInt();
  return QVariant();
}

QVariantMap loadTiledProperties(const QJsonObject& object)
{
  const QJsonArray jsonProperties = object["properties"].toArray();
  QVariantMap properties;

  for (const QJsonValue& descriptor : jsonProperties)
  {
    properties.insert(
      descriptor["name"].toString(),
      tiledPropertyToVariant(descriptor.toObject())
    );
  }
  return properties;
}

