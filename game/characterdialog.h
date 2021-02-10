#ifndef  CHARACTERDIALOG_H
# define CHARACTERDIALOG_H

# include <QObject>
# include <QJSValue>
# include <QJsonDocument>
# include "character.h"

class CharacterDialog : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Character*  player         MEMBER player)
  Q_PROPERTY(Character*  npc            MEMBER npc)
  Q_PROPERTY(QString     stateReference MEMBER stateReference NOTIFY stateReferenceChanged)
  Q_PROPERTY(QString     text           MEMBER text NOTIFY textChanged)
  Q_PROPERTY(QStringList options        MEMBER options NOTIFY optionsChanged)
  Q_PROPERTY(QString     mood           MEMBER mood NOTIFY moodChanged);
  Q_PROPERTY(QString     ambiance       MEMBER ambiance NOTIFY ambianceChanged);
public:
  explicit CharacterDialog(QObject *parent = nullptr);

  void load(const QString& name, Character* player, Character* npc);
  void loadState(const QString& reference);
  void setAmbiance(const QString& value) { ambiance = value; emit ambianceChanged(); }

  Q_INVOKABLE void    selectOption(const QString& key);
  Q_INVOKABLE QString getOptionText(const QString& key);

signals:
  void stateReferenceChanged();
  void textChanged();
  void optionsChanged();
  void moodChanged();
  void ambianceChanged();
  void dialogEnded();

private:
  QJsonDocument data;
  Character*    player;
  Character*    npc;
  QString       stateReference;
  QString       text;
  QStringList   options;
  QString       mood, ambiance;
  QJSValue      script, self;
};

#endif // CHARACTERDIALOG_H
