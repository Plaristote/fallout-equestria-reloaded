#ifndef  COMBATCOMPONENT_H
# define COMBATCOMPONENT_H

# include <QObject>
# include <QQmlListProperty>
# include "../character.h"
# include "visualeffects.h"

# define WORLDTIME_TURN_DURATION_IN_SECONDS 10
# define WORLDTIME_TURN_DURATION WORLDTIME_TURN_DURATION_IN_SECONDS * 1000

class CombatComponent : public VisualEffectsComponent
{
  Q_OBJECT

  Q_PROPERTY(bool combat MEMBER combat NOTIFY combatChanged)
  Q_PROPERTY(QQmlListProperty<Character> combattants READ getQmlCombattants NOTIFY combattantsChanged)
  Q_PROPERTY(int  turn MEMBER combatIterator NOTIFY currentCombattantChanged)
  Q_PROPERTY(bool isPlayerTurn READ isPlayerTurn NOTIFY currentCombattantChanged)
public:
  explicit CombatComponent(QObject* parent = nullptr);

  QQmlListProperty<Character> getQmlCombattants() { return QQmlListProperty<Character>(this, &combattants); }

  void registerDynamicObject(DynamicObject*);
  void unregisterDynamicObject(DynamicObject*);

  void startCombat(Character* character);
  void onNextCombatTurn();
  bool isPlayerTurn() const;

  Q_INVOKABLE bool isCharacterTurn(Character* charcter) const;
  Q_INVOKABLE bool isInCombat(Character* character) const { return combattants.contains(character); }
  Q_INVOKABLE void joinCombat(Character* character);
  Q_INVOKABLE void leaveCombat(Character* character);
  Q_INVOKABLE bool tryToEndCombat();
  Q_INVOKABLE void passTurn(Character* character);

  // TODO write a real check for that
  bool isCombatEnabled() const { return combattants.length() > 1 && combat == true; }

signals:
  void combatChanged();
  void combattantsChanged();
  void currentCombattantChanged();

private slots:
  void onActiveItemChanged();
  virtual void onCharacterDied(Character*);

protected:
  void sortCombattants();
  void removeDeadCombattants();
  void finalizeCharacterTurn(Character*);
  virtual void finalizeRound();
  void initializeCharacterTurn(Character*);

  bool combat = false;
  int combatIterator = 0;
  QList<Character*> combattants;
};

#endif // COMBATCOMPONENT_H
