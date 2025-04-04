#ifndef  OBJECTFACTORY_H
# define OBJECTFACTORY_H

# include <QObject>
# include "objectgroup.h"

class Character;
class StorageObject;
class InventoryItem;
class Doorway;
class Elevator;
class DynamicObject;

class ObjectFactory : public QObject
{
  Q_OBJECT
public:
  ObjectFactory(ObjectGroup*);

  DynamicObject*             loadFromJson(const QJsonObject&) const;
  ObjectGroup*               loadJsonGroup(const QJsonObject&) const;
  Q_INVOKABLE ObjectGroup*   generateGroup(const QString& name) const;
  Q_INVOKABLE Character*     generateCharacter(const QString &name, const QString &characterSheet) const;
  Q_INVOKABLE StorageObject* generateStorageObject(const QString &name) const;
  Q_INVOKABLE InventoryItem* generateInventoryItem(const QString& name, const QString& type, int quantity) const;
  Q_INVOKABLE Doorway*       generateDoorway(const QString &name) const;
  Q_INVOKABLE Elevator*      generateElevator(const QString& name) const;
  Q_INVOKABLE DynamicObject* generateDynamicObject(const QString &name) const;
  Q_INVOKABLE DynamicObject* addBloodStainAt(QPoint position, unsigned char floor) const;

private:
  ObjectGroup* root;
};

#endif // OBJECTFACTORY_H
