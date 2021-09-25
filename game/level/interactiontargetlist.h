#ifndef  INTERACTIONTARGETLIST_H
# define INTERACTIONTARGETLIST_H

# include <QQmlListProperty>
# include "game/character.h"

class InteractionTargetList : public QObject
{
  Q_OBJECT

  Q_PROPERTY(unsigned int cursor READ getCursor NOTIFY cursorChanged)
  Q_PROPERTY(QQmlListProperty<DynamicObject> targets READ getQmlTargets NOTIFY targetsUpdated)
public:
  explicit InteractionTargetList(QObject* parent = nullptr);

  Q_INVOKABLE DynamicObject* nextTarget();
  Q_INVOKABLE DynamicObject* previousTarget();

  virtual void unregisterDynamicObject(DynamicObject* object);
  void reset();
  void findItemTargets(InventoryItem* item, const QList<DynamicObject*>& objects, const QList<Character*>& visibleCharacters);
  void findNearbyTargets(const QList<DynamicObject*>& objects);
signals:
  void targetsUpdated();
  void cursorChanged();

private:
  void afterTargetUpdate();
  const QList<DynamicObject*>& getTargets() const { return targets; }
  QQmlListProperty<DynamicObject> getQmlTargets() { return QML_QLIST_CONSTRUCTOR(DynamicObject, targets); }
  unsigned int getCursor() const { return cursor; }

  QList<DynamicObject*> targets;
  unsigned int          cursor = 0;
};

#endif // INTERACTIONTARGETLIST_H
