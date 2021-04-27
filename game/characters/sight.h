#ifndef  CHARACTERSIGHT_H
# define CHARACTERSIGHT_H

# include "inventory.h"
# include "field_of_view.hpp"

class CharacterSight : public CharacterInventory
{
  Q_OBJECT
  typedef CharacterInventory ParentType;

  Q_PROPERTY(FieldOfView* fieldOfView MEMBER fieldOfView)
public:
  explicit CharacterSight(QObject *parent = nullptr);
  virtual ~CharacterSight();

  Q_INVOKABLE bool  hasLineOfSight(DynamicObject*) const;
  bool              hasLineOfSight(const DynamicObject*) const;
  bool              hasLineOfSight(QPoint target) const;

  static bool       hasSightFrom(const DynamicObject*, QPoint position);
  static bool       hasSightFrom(QPoint target, QPoint position);
  Q_INVOKABLE bool  isSneaking() const { return sneakEnabled; }
  float             getDistance(const DynamicObject*) const;
  Q_INVOKABLE float getDistance(DynamicObject* v) const;
  Q_INVOKABLE float getDistance(int x, int y) const { return getDistance(QPoint(x, y)); }
  float             getDistance(QPoint) const;
  FieldOfView*      getFieldOfView() const { return fieldOfView; }

protected:
  inline void updateFieldOfView(double duration) { fieldOfView->update(duration); }
  inline bool hasLivingEnemiesInSight() const { return fieldOfView->hasLivingEnemiesInSight(); }

private:
  FieldOfView* fieldOfView;
  bool sneakEnabled = false;
};

#endif // CHARACTERSIGHT_H
