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

  Q_INVOKABLE bool  hasLineOfSight(const DynamicObject*) const;
  bool              hasLineOfSight(Point target) const;

  static bool       hasSightFrom(const DynamicObject*, Point position);
  static bool       hasSightFrom(Point target, Point position);
  float             getDistance(const DynamicObject*) const;
  Q_INVOKABLE float getDistance(DynamicObject* v) const;
  Q_INVOKABLE float getDistance(int x, int y) const { return getDistance(QPoint(x, y)); }
  float             getDistance(QPoint) const;
  FieldOfView*      getFieldOfView() const { return fieldOfView; }

private slots:
  void refreshFieldOfView();
  void onRefreshed();
  void onCharacterDetected(Character*);

protected:
  inline void updateFieldOfView(double duration) { fieldOfView->update(duration); }
  inline bool hasLivingEnemiesInSight() const { return fieldOfView->hasLivingEnemiesInSight(); }

private:
  FieldOfView* fieldOfView;
};

#endif // CHARACTERSIGHT_H
