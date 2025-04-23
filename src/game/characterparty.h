#ifndef  CHARACTERPARTY_H
# define CHARACTERPARTY_H

# include <QObject>
# include <QQmlListProperty>
# include "character.h"

class GridComponent;
class TileZone;

class CharacterParty : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged)
  Q_PROPERTY(QString factionName MEMBER factionName NOTIFY factionNameChanged)
  Q_PROPERTY(QQmlListProperty<Character> list READ getQmlCharacters NOTIFY partyChanged)
public:
  explicit CharacterParty(QObject *parent = nullptr);

  static CharacterParty* factory(const QVariantMap& parameters, QObject* parent = nullptr);

  const QString& getName() const { return name; }
  const QString& getFactionName() const { return factionName; }
  void setFactionName(const QString& value) { factionName = value; emit factionNameChanged(); }
  void loadIntoLevel(GridComponent*);
  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  Q_INVOKABLE void createCharacters(const QVariantMap&);
  Q_INVOKABLE void createCharacter(const QString &name, const QVariantMap& parameters);
  Q_INVOKABLE void addCharacter(Character*);
  Q_INVOKABLE void removeCharacter(Character*);
  Q_INVOKABLE bool containsCharacter(const Character*) const;
  Q_INVOKABLE Character* get(const QString& name);
  const QList<Character*>& getCharacters() const { return list; }
  Q_INVOKABLE void addExperience(unsigned int value);
  Q_INVOKABLE void requireJoinCombat();
  void             joinCombat();
  Q_INVOKABLE Character* find(QJSValue callback) const;

  Q_INVOKABLE Character* mostSkilledAt(const QByteArray& stat) const;
  Q_INVOKABLE Character* leastSkilledAt(const QByteArray& stat) const;
  Q_INVOKABLE int highestStatistic(const QByteArray& stat) const;
  Q_INVOKABLE int lowestStatistic(const QByteArray& stat) const;

  bool insertIntoZone(GridComponent*, TileZone*) const;
  bool insertIntoZone(GridComponent*, const QString& zoneName) const;
  Q_INVOKABLE bool insertIntoZone(GridComponent*, QJSValue) const;
  Q_INVOKABLE void extractFromLevel(GridComponent*);

  QQmlListProperty<Character> getQmlCharacters() { return QML_QLIST_CONSTRUCTOR(Character, list); }

signals:
  void nameChanged();
  void partyChanged();
  void factionNameChanged();

private:
  void updateFaction();
  void useCharacterFaction(const Character*);
  void enforceFactionOn(Character*);
  void rollbackFactionOn(Character*);

  QString           name;
  QString           factionName;
  QList<Character*> list;
};

#endif // CHARACTERPARTY_H
