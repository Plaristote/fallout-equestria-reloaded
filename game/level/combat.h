#ifndef  COMBATCOMPONENT_H
# define COMBATCOMPONENT_H

# include <QObject>
# include <QQmlListProperty>
# include "../character.h"
# include "interaction.h"

class CombatComponent : public InteractionComponent
{
  Q_OBJECT

  Q_PROPERTY(bool combat MEMBER combat NOTIFY combatChanged)
  Q_PROPERTY(QQmlListProperty<Character> combattants READ getQmlCombattants NOTIFY combattantsChanged)
  Q_PROPERTY(int  turn MEMBER combatIterator NOTIFY currentCombattantChanged)
public:
  explicit CombatComponent(QObject* parent = nullptr);

  QQmlListProperty<Character> getQmlCombattants() { return QQmlListProperty<Character>(this, &combattants); }

  /*
   * TODO: find a way for characters to schedule orders.
   *
  void update(qint64 delta)
  {
    if (combat)
    {
    }
  }

  void updateCombat(qint64 delta)
  {

  }
  */

  Q_INVOKABLE void joinCombat(Character* character);
  Q_INVOKABLE void leaveCombat(Character* character);
  Q_INVOKABLE bool tryToEndCombat();

  // TODO write a real check for that
  bool isCombatEnabled() const { return combattants.length() > 1 && combat == true; }

signals:
  void combatChanged();
  void combattantsChanged();
  void currentCombattantChanged();

private:
  void sortCombattants();
  void removeDeadCombattants();

  bool combat = false;
  int combatIterator = 0;
  QList<Character*> combattants;
};

#endif // COMBATCOMPONENT_H
