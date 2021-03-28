#ifndef  CHARACTERINVENTORY_H
# define CHARACTERINVENTORY_H

# include "diplomacy.h"

class CharacterInventory : public CharacterDiplomacy
{
  Q_OBJECT
  typedef CharacterDiplomacy ParentType;
public:
  explicit CharacterInventory(QObject *parent = nullptr);

private slots:
  void updateInventorySlots();
  void initializeEmptySlots();
  void initializeEmptySlot(const QString& name);

private:
  QString getDefaultItemForSlot(const QString& name);
};

#endif // CHARACTERINVENTORY_H
