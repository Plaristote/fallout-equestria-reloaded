#ifndef  INTERACTIONCOMPONENT_H
# define INTERACTIONCOMPONENT_H

# include <QObject>
# include "grid.h"
# include "../dynamicobject.h"
# include "../inventoryitem.h"

class CharacterDialog;
class LootingController;

class InteractionComponent : public GridComponent
{
  Q_OBJECT

  Q_PROPERTY(int mouseMode MEMBER mouseMode NOTIFY mouseModeChanged)
public:
  enum MouseMode {
    MovementCursor    = 0,
    InteractionCursor = 1,
    TargetCursor      = 2
  };

  explicit InteractionComponent(QObject *parent = nullptr);

  void registerDynamicObject(DynamicObject*);
  void unregisterDynamicObject(DynamicObject*);

  bool             openInteractionMenu(DynamicObject* target);
  Q_INVOKABLE void interactOrderReceived(DynamicObject* target, const QString& interactionType);
  Q_INVOKABLE void swapMouseMode();
  Q_INVOKABLE void setActiveItem(InventoryItem*);
  Q_INVOKABLE void objectClicked(DynamicObject*);

signals:
  void mouseModeChanged();
  void interactionRequired(DynamicObject* target, QStringList options);
  void startDialog(CharacterDialog*);
  void startLooting(LootingController*);
  void activeItemChanged();

protected slots:
  void startPendingInteraction();

private:
  void useItemOn(DynamicObject* object);
  void initializeDialog(Character* npc);
  void initializeLooting(StorageObject*);

protected:
  QMetaObject::Connection        playerReachedDestinationObserver;
  QPair<DynamicObject*, QString> pendingInteraction;
  InventoryItem*                 activeItem = nullptr;
  int                            mouseMode = MovementCursor;
};

#endif // INTERACTIONCOMPONENT_H
