#ifndef  INTERACTIONCOMPONENT_H
# define INTERACTIONCOMPONENT_H

# include <QObject>
# include "playervisibility.h"
# include "../dynamicobject.h"
# include "../objects/inventoryitem.h"

class CharacterDialog;
class LootingController;

class InteractionComponent : public PlayerVisibilityComponent
{
  Q_OBJECT
  typedef PlayerVisibilityComponent ParentType;

  Q_PROPERTY(int mouseMode MEMBER mouseMode NOTIFY mouseModeChanged)
  Q_PROPERTY(int targetMode READ getTargetMode NOTIFY mouseModeChanged)
  Q_PROPERTY(InventoryItem* activeItem READ getActiveItem NOTIFY activeItemChanged)
public:
  static int movementModeOption;

  enum MouseMode
  {
    MovementCursor    = 0,
    InteractionCursor = 1,
    TargetCursor      = 2,
    WaitCursor        = 3
  };

  enum TargetMode
  {
    AnyTarget       = 0,
    CharacterTarget = 1,
    ZoneTarget      = 2
  };

  enum MovementMode
  {
    MixedMovementMode = 0,
    RunMovementMode   = 1,
    WalkMovementMode  = 2
  };

  explicit InteractionComponent(QObject *parent = nullptr);

  void registerDynamicObject(DynamicObject*);
  void unregisterDynamicObject(DynamicObject*);

  bool             openInteractionMenu(DynamicObject* target);
  Q_INVOKABLE void openCountdownDialog(InventoryItem* item);
  Q_INVOKABLE void interactOrderReceived(DynamicObject* target, const QString& interactionType);
  Q_INVOKABLE void swapMouseMode();
  Q_INVOKABLE void enableWaitingMode(bool);
  Q_INVOKABLE void setActiveItem(const QString&);
  inline InventoryItem* getActiveItem() const { return activeItem; }
  Q_INVOKABLE void objectClicked(DynamicObject*);
  Q_INVOKABLE void useSkill(const QString& skill);
  Q_INVOKABLE void useSkillOn(Character* user, DynamicObject* object, const QString& skill);
  Q_INVOKABLE void movePlayerTo(int x, int y);
  Q_INVOKABLE void tileClicked(int x, int y);
  int              getTargetMode() const;
  void             setDefaultMovementMode();

  Q_INVOKABLE DynamicObject*   getObjectAt(int posX, int posY) const;
  DynamicObject*               getObjectAt(QPoint point) const { return getObjectAt(point.x(), point.y()); }

  void             useItemOn(DynamicObject* object);
  Q_INVOKABLE void useItemOn(InventoryItem* item, DynamicObject* target);
  void             useItemOn(Character* user, InventoryItem* item, DynamicObject* target);
  void             useItemAt(int x, int y);
  Q_INVOKABLE void useItemAt(InventoryItem* item, int x, int y);
  void             useItemAt(Character* user, InventoryItem* item, int x, int y);
  void             pickUpItem(Character*, InventoryItem*);
  Q_INVOKABLE void initializeDialog(Character* npc);
  Q_INVOKABLE void initializeLooting(StorageObject*);

signals:
  void mouseModeChanged();
  void interactionRequired(DynamicObject* target, QStringList options);
  void countdownRequired(InventoryItem* item);
  void startDialog(CharacterDialog*);
  void startLooting(LootingController*);
  void activeItemChanged();
  void playerMovingTo(QPoint);

private slots:
  void onActiveItemChanged();

private:
  int getInteractionDistance(DynamicObject* target, const QString& interactionType);

protected:
  QString        activeItemSlot, activeSkill;
  InventoryItem* activeItem = nullptr;
  int            mouseMode = MovementCursor;
};

#endif // INTERACTIONCOMPONENT_H
