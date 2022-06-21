#ifndef  TARGETCOMPONENT_H
# define TARGETCOMPONENT_H

# include "cursor.h"
# include "interactiontargetlist.h"

class TargetComponent : public CursorComponent
{
  Q_OBJECT
  typedef CursorComponent ParentType;

  Q_PROPERTY(int targetMode READ getTargetMode NOTIFY mouseModeChanged)
  Q_PROPERTY(InteractionTargetList* targetList READ getTargetList CONSTANT)
public:
  enum TargetMode
  {
    AnyTarget       = 0,
    CharacterTarget = 1,
    ZoneTarget      = 2
  };
  Q_ENUM(TargetMode)

  explicit TargetComponent(QObject *parent = nullptr);

  virtual void registerDynamicObject(DynamicObject*) override;
  virtual void unregisterDynamicObject(DynamicObject*) override;
  virtual void swapMouseMode() override;
  virtual int  getTargetMode() const { return AnyTarget; }

protected:
  bool isPotentialTarget(const DynamicObject&) const override;
  InteractionTargetList* getTargetList() { return &targetList; }
  InteractionTargetList targetList;
};

#endif // TARGETCOMPONENT_H
