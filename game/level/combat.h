#ifndef  COMBATCOMPONENT_H
# define COMBATCOMPONENT_H

# include <QObject>
# include <QQmlListProperty>
# include "../character.h"
# include "visualeffects.h"

# define WORLDTIME_TURN_DURATION_IN_SECONDS 8
# define WORLDTIME_TURN_DURATION WORLDTIME_TURN_DURATION_IN_SECONDS * 1000

class CombatComponent : public VisualEffectsComponent
{
  Q_OBJECT
  typedef VisualEffectsComponent ParentType;

  Q_PROPERTY(bool combat MEMBER combat NOTIFY combatChanged)
  Q_PROPERTY(QQmlListProperty<Character> combattants READ getQmlCombattants NOTIFY combattantsChanged)
  Q_PROPERTY(int  turn MEMBER combatIterator NOTIFY currentCombattantChanged)
  Q_PROPERTY(bool isPlayerTurn READ isPlayerTurn NOTIFY currentCombattantChanged)
public:
  static double combatSpeedOption;

  explicit CombatComponent(QObject* parent = nullptr);

  QQmlListProperty<Character> getQmlCombattants() { return QML_QLIST_CONSTRUCTOR(Character, combattants); }

  virtual void registerDynamicObject(DynamicObject*) override;
  virtual void unregisterDynamicObject(DynamicObject*) override;

  void startCombat(Character* character);
  void onNextCombatTurn();
  bool isPlayerTurn() const;

  Q_INVOKABLE bool isCharacterTurn(Character* charcter) const;
  Q_INVOKABLE bool isInCombat(Character* character) const { return combattants.contains(character); }
  Q_INVOKABLE void joinCombat(Character* character);
  Q_INVOKABLE void leaveCombat(Character* character);
  Q_INVOKABLE bool tryToEndCombat();
  Q_INVOKABLE void passTurn(Character* character);

  bool isCombatEnabled() const;
  bool nothingHappenedInCombatTurn() const;

signals:
  void combatChanged();
  void combattantsChanged();
  void currentCombattantChanged();
  void scheduleCombatEnd();

protected slots:
  virtual void onCombatStateChanged();
private slots:
  void onCurrentCombattantChanged();
  void onCombattantsChanged();
  void onActiveItemChanged();
  void updateWaitingMode();
  virtual void onCharacterDied(Character*) override;

protected:
  void sortCombattants();
  void removeDeadCombattants();
  void finalizeCharacterTurn(Character*);
  virtual void finalizeRound();
  void initializeCharacterTurn(Character*);

  void initializeArmorClassBonus(Character*);
  void finalizeArmorClassBonus(Character*);
  void finalizeAllArmorClassBonus();

  bool combat = false;
  int combatIterator = 0;
  QList<Character*> combattants;
  QMap<Character*, int> armorClassBonuses;
};

#endif // COMBATCOMPONENT_H
