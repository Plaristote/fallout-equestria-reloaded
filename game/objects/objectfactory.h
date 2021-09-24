#ifndef  OBJECTFACTORY_H
# define OBJECTFACTORY_H

# include <QObject>
# include "objectgroup.h"

class Character;
class StorageObject;
class InventoryItem;
class Doorway;
class DynamicObject;

class ObjectFactory : public QObject
{
  Q_OBJECT
public:
  ObjectFactory(ObjectGroup*);

  Q_INVOKABLE Character*     generateCharacter(const QString &name, const QString &characterSheet);
  Q_INVOKABLE StorageObject* generateStorageObject(const QString &name);
  Q_INVOKABLE InventoryItem* generateInventoryItem(const QString& name, const QString& type, int quantity);
  Q_INVOKABLE Doorway*       generateDoorway(const QString &name);
  Q_INVOKABLE DynamicObject* generateDynamicObject(const QString &name);

private:
  ObjectGroup* root;
};

#endif // OBJECTFACTORY_H
