#include "properties.h"
#include <QJsonObject>
#include <QJsonArray>

static QVariant tiledPropertyToVariant(const QJsonValue& value, const QString& typeName)
{
  if (typeName == "bool")
    return value.toBool();
  if (typeName == "string" || typeName == "color" || typeName == "file")
    return value.toString();
  if (typeName == "float")
    return value.toDouble();
  if (typeName == "int")
    return value.toInt();
  return QVariant();
}

static QVariantMap loadTiledProperties_v1_8(const QJsonObject& object)
{
  const QJsonArray jsonProperties = object["properties"].toArray();
  QVariantMap properties;

  for (const QJsonValue& descriptor : jsonProperties)
  {
    QJsonObject asObject = descriptor.toObject();
    properties.insert(
      descriptor["name"].toString(),
      tiledPropertyToVariant(asObject["value"], asObject["type"].toString())
    );
  }
  return properties;
}

static QVariantMap loadTiledProperties_v1_9(const QJsonObject& object)
{
  const QJsonObject jsonProperties = object["properties"].toObject();
  const QJsonObject propertyTypes  = object["propertytypes"].toObject();
  QVariantMap properties;

  for (const QString& key : jsonProperties.keys())
  {
    properties.insert(
      key,
      tiledPropertyToVariant(jsonProperties[key], propertyTypes[key].toString())
    );
  }
  return properties;
}

QVariantMap loadTiledProperties(const QJsonObject& object, const QString& tiledVersion)
{
  if (tiledVersion.startsWith("1") && !tiledVersion.startsWith("1.9"))
    return loadTiledProperties_v1_8(object);
  return loadTiledProperties_v1_9(object);
}
