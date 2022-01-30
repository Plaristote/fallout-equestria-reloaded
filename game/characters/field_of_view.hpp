#ifndef  FIELD_OF_VIEW_HPP
# define FIELD_OF_VIEW_HPP

# include <QObject>
# include <QQmlListProperty>
# include <QJSValue>
# define FLAG_CHARACTER_SNEAK 1
# define FOV_TTL              5

class Level;
class Character;

class FieldOfView : public QObject
{
  Q_OBJECT

  typedef QList<Character*>           CharacterList;
  typedef QQmlListProperty<Character> QmlCharacterList;

  struct Entry
  {
    Entry(Character* character) : character(character), time_to_live(FOV_TTL)
    {
    }

    bool       operator==(const Character* comp) const { return (character == comp); }
    Character& operator*(void) const { return (*character); }

    Character* character;
    char       time_to_live;
  };

public:
  FieldOfView(Character& character);
  ~FieldOfView();

  void                 SetIntervalDurationFromStatistics(void);

  Q_INVOKABLE void     detectCharacters();
  Q_INVOKABLE bool     isDetected(const Character*) const;
  Q_INVOKABLE bool     hasLivingEnemiesInSight(void) const;

  Q_INVOKABLE QJSValue getCharactersInRange() const;
  Q_INVOKABLE QJSValue getEnemies() const;
  Q_INVOKABLE QJSValue getAllies() const;
  Q_INVOKABLE QJSValue getNonHostiles() const;
  Q_INVOKABLE QJSValue getCharacters() const;

  Q_INVOKABLE void     setEnemyDetected(Character* enemy);
  Q_INVOKABLE void     setCharacterDetected(Character* character);
  CharacterList        GetCharactersInRange(void)  const;
  CharacterList        GetDetectedEnemies(void)    const;
  CharacterList        GetDetectedAllies(void)     const;
  CharacterList        GetDetectedNonHostile(void) const;
  CharacterList        GetDetectedCharacters(void) const;
  float                GetRadius(void)             const;

  void                 update(qint64 delta);
  void                 runTask();
  void                 reset();
  void                 removeCharacter(Character*);

signals:
  void                 updated();
  void                 refreshed();
  void                 characterDetected(Character*);

protected:
  void                 LoseTrackOfCharacters(std::list<Entry>&);
  bool                 CheckIfEnemyIsDetected(Character& enemy)                  const;
  bool                 CheckIfSneakingEnemyIsDetected(Character& enemy)          const;
  void                 InsertOrUpdateCharacterInList(Character&, std::list<Entry>&);
  bool                 IsCharacterInList(const Character*, const std::list<Entry>&)    const;
  void                 AppendEntriesToCharacterList(const std::list<Entry>&, CharacterList&) const;
  CharacterList        GetDetectedCharactersMatching(std::function<bool (Character*)>) const;

private:
  qint64               interval, timeLeft;
  Character&           character;
  std::list<Entry>     detected_enemies;
  std::list<Entry>     detected_characters;
};

#endif
