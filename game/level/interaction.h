#ifndef  INTERACTIONCOMPONENT_H
# define INTERACTIONCOMPONENT_H

# include <QObject>
# include "grid.h"
# include "../dynamicobject.h"

class CharacterDialog;
class LootingController;

class InteractionComponent : public GridComponent
{
  Q_OBJECT
public:
  explicit InteractionComponent(QObject *parent = nullptr);

  void registerDynamicObject(DynamicObject*);
  void unregisterDynamicObject(DynamicObject*);

  bool             openInteractionMenu(DynamicObject* target);
  Q_INVOKABLE void interactOrderReceived(DynamicObject* target, const QString& interactionType);

signals:
  void interactionRequired(DynamicObject* target, QStringList options);
  void startDialog(CharacterDialog*);
  void startLooting(LootingController*);

protected slots:
  void startPendingInteraction();

private:
  void initializeDialog(Character* npc);
  void initializeLooting(StorageObject*);

protected:
  QMetaObject::Connection        playerReachedDestinationObserver;
  QPair<DynamicObject*, QString> pendingInteraction;
};

#endif // INTERACTIONCOMPONENT_H
