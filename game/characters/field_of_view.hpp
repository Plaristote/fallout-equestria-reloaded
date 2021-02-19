#ifndef  FIELD_OF_VIEW_HPP
# define FIELD_OF_VIEW_HPP

# include <QObject>
# include <QQmlListProperty>
# define FLAG_CHARACTER_SNEAK 1
# define FOV_TTL              5
# define FOV_QML_READER(methodName) \
  QmlCharacterList qml##methodName() \
  { \
    qmlTemporaryList = methodName(); \
    return QmlCharacterList(this, &qmlTemporaryList); \
  }

class Level;
class Character;

class FieldOfView : public QObject
{
  Q_OBJECT

  typedef QList<Character*>           CharacterList;
  typedef QQmlListProperty<Character> QmlCharacterList;

  Q_PROPERTY(QmlCharacterList charactersInRange READ qmlGetCharactersInRange)
  Q_PROPERTY(QmlCharacterList enemies           READ qmlGetDetectedEnemies)
  Q_PROPERTY(QmlCharacterList allies            READ qmlGetDetectedAllies)
  Q_PROPERTY(QmlCharacterList nonHostiles       READ qmlGetDetectedNonHostile)
  Q_PROPERTY(QmlCharacterList characters        READ qmlGetDetectedCharacters)

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

  Q_INVOKABLE bool     isDetected(const Character*) const;
  Q_INVOKABLE bool     hasLivingEnemiesInSight(void) const;

  Q_INVOKABLE void     setEnemyDetected(Character* enemy);
  Q_INVOKABLE void     setCharacterDetected(Character* character);
  CharacterList        GetCharactersInRange(void)  const;
  CharacterList        GetDetectedEnemies(void)    const;
  CharacterList        GetDetectedAllies(void)     const;
  CharacterList        GetDetectedNonHostile(void) const;
  CharacterList        GetDetectedCharacters(void) const;
  float                GetRadius(void)             const;

  void                 update(qint64 delta);
  void                 reset();

protected:
  void                 LoseTrackOfCharacters(std::list<Entry>&);
  void                 DetectCharacters(void);
  bool                 CheckIfEnemyIsDetected(const Character& enemy)                  const;
  bool                 CheckIfSneakingEnemyIsDetected(const Character& enemy)          const;
  void                 InsertOrUpdateCharacterInList(Character&, std::list<Entry>&);
  bool                 IsCharacterInList(const Character*, const std::list<Entry>&)    const;
  void                 AppendEntriesToCharacterList(const std::list<Entry>&, CharacterList&) const;
  CharacterList        GetDetectedCharactersMatching(std::function<bool (Character*)>) const;

private:
  qint64               interval, timeLeft;
  Character&           character;
  std::list<Entry>     detected_enemies;
  std::list<Entry>     detected_characters;

  CharacterList qmlTemporaryList;
  FOV_QML_READER(GetCharactersInRange)
  FOV_QML_READER(GetDetectedEnemies)
  FOV_QML_READER(GetDetectedAllies)
  FOV_QML_READER(GetDetectedNonHostile)
  FOV_QML_READER(GetDetectedCharacters)
};

#endif
