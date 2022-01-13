#ifndef  ACTIONSCOMPONENT_H
# define ACTIONSCOMPONENT_H

# include "targetcomponent.h"

class ActionsComponent : public TargetComponent
{
  Q_OBJECT
  typedef TargetComponent ParentType;

  Q_PROPERTY(InventoryItem* activeItem READ getActiveItem NOTIFY activeItemChanged)
public:
  enum InteractionType
  {
    NoInteraction,
    ItemUse,
    SkillUse,
    SpellUse
  };

  explicit ActionsComponent(QObject *parent = nullptr);

  Q_INVOKABLE void setActiveItem(const QString&);
  inline InventoryItem* getActiveItem() const { return activeItem; }

  Q_INVOKABLE void useSpell(const QString& spell);
  Q_INVOKABLE void useSpellOn(Character* user, DynamicObject* object, const QString& spell);
  Q_INVOKABLE void useSpellAt(Character* user, int posX, int posY, const QString& spell);
  Q_INVOKABLE void useSkill(const QString& skill);
  Q_INVOKABLE void useSkillOn(Character* user, DynamicObject* object, const QString& skill);
  void             useItemOn(DynamicObject* object);
  Q_INVOKABLE void useItemOn(InventoryItem* item, DynamicObject* target, const QString& mode = "");
  void             useItemOn(Character* user, InventoryItem* item, DynamicObject* target, const QString& mode = "");
  void             useItemAt(int x, int y);
  Q_INVOKABLE void useItemAt(InventoryItem* item, int x, int y);
  void             useItemAt(Character* user, InventoryItem* item, int x, int y);
  void             pickUpItem(Character*, InventoryItem*);
  Q_INVOKABLE bool canSneak(Character*);
  Q_INVOKABLE bool useSneak(Character*);

signals:
  void activeItemChanged();

protected slots:
  void onActiveItemChanged();
  void resetInteractionMode();

protected:
  QString               activeItemSlot, activeSkill;
  InventoryItem*        activeItem = nullptr;
  InteractionType       interactionType = NoInteraction;

};

#endif // ACTIONSCOMPONENT_H
