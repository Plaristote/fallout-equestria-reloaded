#include "storableobject.h"

StorableObject::StorableObject(QObject *parent) : QObject(parent)
{

}

void StorableObject::load(const QJsonObject& data)
{
  dataStore = data["dataStore"].toObject();
}

void StorableObject::save(QJsonObject& data) const
{
  data["dataStore"] = dataStore;
}
