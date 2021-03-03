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
  Q_INVOKABLE void setActiveItem(const QString&);
  Q_INVOKABLE void objectClicked(DynamicObject*);
  Q_INVOKABLE void useSkill(const QString& skill);
  Q_INVOKABLE void useSkillOn(Character* user, DynamicObject* object, const QString& skill);

  void useItemOn(DynamicObject* object);
  void initializeDialog(Character* npc);
  void initializeLooting(StorageObject*);

signals:
  void mouseModeChanged();
  void interactionRequired(DynamicObject* target, QStringList options);
  void startDialog(CharacterDialog*);
  void startLooting(LootingController*);
  void activeItemChanged();

protected:
  QString        activeItemSlot, activeSkill;
  InventoryItem* activeItem = nullptr;
  int            mouseMode = MovementCursor;
};

#endif // INTERACTIONCOMPONENT_H
