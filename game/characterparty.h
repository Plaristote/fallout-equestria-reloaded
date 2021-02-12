#ifndef  CHARACTERPARTY_H
# define CHARACTERPARTY_H

# include <QObject>
# include <QQmlListProperty>
# include "character.h"

class LevelTask;
class TileZone;

class CharacterParty : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged)
  Q_PROPERTY(QQmlListProperty<Character> list READ getQmlCharacters NOTIFY partyChanged)
public:
  explicit CharacterParty(QObject *parent = nullptr);

  void loadIntoLevel(LevelTask*);
  void load(const QJsonObject&);
  void save(QJsonObject&);

  Q_INVOKABLE void addCharacter(Character*);
  Q_INVOKABLE void removeCharacter(Character*);
  Q_INVOKABLE void removeCharacter(const QString& name);
  Q_INVOKABLE bool containsCharacter(Character*);
  Q_INVOKABLE Character* get(const QString& name);
  const QList<Character*>& getCharacters() const { return list; }
  QQmlListProperty<Character> getQmlCharacters() { return QQmlListProperty<Character>(this, &list); }

  bool insertIntoZone(LevelTask*, TileZone*);
  Q_INVOKABLE bool insertIntoZone(LevelTask*, const QString& zoneName);
  Q_INVOKABLE void extractFromLevel(LevelTask*);

signals:
  void nameChanged();
  void partyChanged();

private:
  QString           name;
  QList<Character*> list;
};

#endif // CHARACTERPARTY_H
