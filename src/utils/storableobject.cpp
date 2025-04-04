#include "storableobject.h"

bool shouldSaveVariables();

StorableObject::StorableObject(QObject *parent) : QObject(parent)
{

}

void StorableObject::load(const QJsonObject& data)
{
  dataStore = data["dataStore"].toObject();
}

void StorableObject::save(QJsonObject& data) const
{
  if (shouldSaveVariables())
    data["dataStore"] = dataStore;
}

QVariantList StorableObject::getVariables(const QStringList& list) const
{
  QVariantList result;

  for (const QString& name : list)
    result.push_back(getVariable(name));
  return result;
}
