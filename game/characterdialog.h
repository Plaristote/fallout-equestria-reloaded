#ifndef  CHARACTERDIALOG_H
# define CHARACTERDIALOG_H

# include <QObject>
# include <QJSValue>
# include <QJsonDocument>
# include "character.h"
# include "i18n.h"

class CharacterDialog : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Character*  player         MEMBER player)
  Q_PROPERTY(Character*  npc            MEMBER npc)
  Q_PROPERTY(QString     stateReference MEMBER stateReference NOTIFY stateReferenceChanged)
  Q_PROPERTY(QString     text           MEMBER text NOTIFY textChanged)
  Q_PROPERTY(QStringList options        MEMBER options NOTIFY optionsChanged)
  Q_PROPERTY(QString     mood           MEMBER mood NOTIFY moodChanged);
  Q_PROPERTY(QString     ambiance       MEMBER ambiance NOTIFY ambianceChanged)
  Q_PROPERTY(QStringList stateList      READ   getStateList NOTIFY stateListChanged)
  Q_PROPERTY(QStringList answerList     READ   getAnswerList NOTIFY answerListChanged)
public:
  explicit CharacterDialog(QObject *parent = nullptr);

  Q_INVOKABLE void load(const QString& name, Character* player, Character* npc);
  Q_INVOKABLE void loadState(const QString& reference);
  void setAmbiance(const QString& value) { ambiance = value; emit ambianceChanged(); }

  Q_INVOKABLE void    selectOption(const QString& key);
  Q_INVOKABLE QString getOptionText(const QString& key);
  Q_INVOKABLE bool    isOptionAvailable(const QString& answer);
  QStringList         getStateList() const;
  QStringList         getAnswerList() const;

  QString             t(const QString& name);

signals:
  void stateReferenceChanged();
  void textChanged();
  void optionsChanged();
  void moodChanged();
  void ambianceChanged();
  void dialogEnded();
  void stateListChanged();
  void answerListChanged();
  void ready();

protected:
  void initializeStateHook(QString& text, QStringList& answers);
  virtual void loadOption(const QString& answer);
  QString getNextState(const QString& answer);
  QString getEntryPoint();

  ScriptController* script = nullptr;
  QJsonObject       data;
  Character*        player;
  Character*        npc;
  QString           stateReference;
  QString           text;
  QStringList       options;
  QString           mood, ambiance;
  QString           translationGroup;
};

#endif // CHARACTERDIALOG_H
