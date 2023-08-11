#ifndef  CHARACTERDIALOG_H
# define CHARACTERDIALOG_H

# include <QObject>
# include <QJSValue>
# include <QJsonDocument>
# include "character.h"
# include "i18n.h"
# include "bartercontroller.h"
# include "dialog/data.h"

class CharacterDialog : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Character*        player         MEMBER player CONSTANT)
  Q_PROPERTY(DynamicObject*    npc            MEMBER npc CONSTANT)
  Q_PROPERTY(QString           name           READ getName NOTIFY nameChanged)
  Q_PROPERTY(QString           stateReference MEMBER stateReference NOTIFY stateReferenceChanged)
  Q_PROPERTY(QString           text           MEMBER text NOTIFY textChanged)
  Q_PROPERTY(QStringList       options        MEMBER options NOTIFY optionsChanged)
  Q_PROPERTY(QString           mood           MEMBER mood NOTIFY moodChanged);
  Q_PROPERTY(QString           ambiance       MEMBER ambiance NOTIFY ambianceChanged)
  Q_PROPERTY(QStringList       stateList      READ   getStateList NOTIFY stateListChanged)
  Q_PROPERTY(QStringList       answerList     READ   getAnswerList NOTIFY answerListChanged)
  Q_PROPERTY(BarterController* barter         READ   getBarterController NOTIFY barterControllerChanged)
  Q_PROPERTY(QStringList       stateHistory   READ   getStateHistory NOTIFY stateReferenceChanged)
public:
  explicit CharacterDialog(QObject *parent = nullptr);
  virtual ~CharacterDialog();

  Q_INVOKABLE bool load(const QString& name, Character* player, DynamicObject* npc);
  Q_INVOKABLE void loadState(const QString& reference);
  void setAmbiance(const QString& value) { ambiance = value; emit ambianceChanged(); }
  Q_INVOKABLE QJSValue getScriptObject() const { return script ? script->getObject() : QJSValue(); }
  ScriptController* getScript() const { return script; }

  Q_INVOKABLE void    selectOption(const QString& key);
  Q_INVOKABLE QString getOptionText(const QString& key);
  Q_INVOKABLE bool    isOptionAvailable(const QString& answer);
  QStringList         getStateList() const;
  QStringList         getAnswerList() const;
  QString             getName() const;
  const QStringList&  getStateHistory() const { return stateHistory; }
  BarterController*   getBarterController() const { return barter; }
  Q_INVOKABLE bool    tryToBarter();

  Q_INVOKABLE QString t(const QString& name, const QVariantMap& = {});

signals:
  void stateReferenceChanged();
  void textChanged();
  void optionsChanged();
  void moodChanged();
  void ambianceChanged();
  void barterStarted();
  void barterEnded();
  void dialogEnded();
  void stateListChanged();
  void answerListChanged();
  void barterControllerChanged();
  void nameChanged();
  void ready();

private slots:
  void onBarterEnded();

protected:
  virtual void loadOption(const QString& answer);
  QString getNextState(const QString& answer);
  QString getEntryPoint();

  DialogData        data;
  ScriptController* script = nullptr;
  BarterController* barter = nullptr;
  Character*        player;
  DynamicObject*    npc;
  QString           stateReference;
  QString           text;
  QStringList       options;
  QString           mood, ambiance;
  QString           translationGroup;
  QStringList       stateHistory;
};

#endif // CHARACTERDIALOG_H
