#ifndef  DIALOGSTATE_H
# define DIALOGSTATE_H

# include "answer.h"
# include <QStringList>
# include <QJSValue>

class DialogData;

struct DialogState
{
  QString text;
  QString mood;
  QStringList answers;
};

class DialogStateData
{
public:
  DialogStateData(DialogData& data);

  void load(const QString& symbol, const QJsonObject&);
  void save(QJsonObject&) const;

  const QString& getSymbol() const { return symbol; }
  void setSymbol(const QString& value) { symbol = value; }
  DialogState setAsCurrentState(CharacterDialog&);
  void setLocalTranslationPath(const QString& value) { localTranslationPath = value; }
  void setDefaultAnswers(const QStringList& value) { defaultAnswers = value; }
  void setMood(const QString& value) { mood = value; }
  void setTriggerHook(const QString& value) { triggerHook = QJSValue(value); }
  QString getDefaultMood() const { return mood; }
  QString getDefaultLocalTranslationPath() const { return localTranslationPath; }
  QString getTriggerHookAsString() const { return triggerHook.isString() ? triggerHook.toString() : QString(); }
  const QStringList& getDefaultAnswers() const { return defaultAnswers; }

private:
  DialogState loadStateFromScriptObject(DialogState, QJSValue);

  DialogData& data;
  QString     symbol;
  QString     localTranslationPath;
  QStringList defaultAnswers;
  QString     mood;
  QJSValue    triggerHook;
};


#endif
