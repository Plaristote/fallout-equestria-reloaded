#ifndef  INTERACTIONCOMPONENT_H
# define INTERACTIONCOMPONENT_H

# include <QObject>
# include "playervisibility.h"
# include "../dynamicobject.h"
# include "../objects/inventoryitem.h"
# include "interactiontargetlist.h"
# include "actions.h"

class CharacterDialog;
class LootingController;

class InteractionComponent : public ActionsComponent
{
  Q_OBJECT
  typedef ActionsComponent ParentType;
public:
  explicit InteractionComponent(QObject *parent = nullptr);

  virtual void registerDynamicObject(DynamicObject*) override;
  virtual void unregisterDynamicObject(DynamicObject*) override;

  bool             openInteractionMenu(DynamicObject* target);
  Q_INVOKABLE void openCountdownDialog(InventoryItem* item);
  Q_INVOKABLE void interactOrderReceived(DynamicObject* target, const QString& interactionType);
  Q_INVOKABLE void objectClicked(DynamicObject*);
  Q_INVOKABLE void tileClicked(int x, int y);
  void             targetTileClicked(int x, int y);
  int              getTargetMode() const override;
  Q_INVOKABLE void initializeDialog(Character* npc);
  Q_INVOKABLE void initializeDialog(DynamicObject* object, const QString& dialogName, const QString& state = "");
  Q_INVOKABLE void initializeLooting(StorageObject*);

signals:
  void interactionRequired(DynamicObject* target, QStringList options);
  void countdownRequired(InventoryItem* item);
  void startDialog(CharacterDialog*);
  void startLooting(LootingController*);

private:
  int getInteractionDistance(DynamicObject* target, const QString& interactionType);
};

#endif // INTERACTIONCOMPONENT_H
