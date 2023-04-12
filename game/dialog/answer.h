#ifndef  DIALOG_ANSWER_H
# define DIALOG_ANSWER_H

# include <QJsonObject>
# include <QJSValue>

class CharacterDialog;

class DialogAnswer
{
public:
  void load(const QString& symbol, const QJsonObject& data);
  void load(const QJSValue&);
  void save(QJsonObject&) const;

  QString getSymbol() const { return symbol; }
  QString getText(CharacterDialog&);
  QString trigger(CharacterDialog&);
  bool    isAvailable(CharacterDialog&);

  void    setSymbol(const QString& value) { symbol = value; }
  void    setLocalTranslationPath(const QString& value) { localTranslationPath = value; }
  QString getLocalTranslationPath() const { return localTranslationPath; }
  void    setDefaultNextState(const QString& value) { defaultNextState = value; }
  QString getDefaultNextState() const { return defaultNextState; }
  void    setAvailableHook(const QString& value) { availableHook = QJSValue(value); }
  QString getAvailableHook() const { return availableHook.isString() ? availableHook.toString() : QString(); }
  void    setTextHook(const QString& value) { textHook = QJSValue(value); }
  QString getTextHook() const { return textHook.isString() ? textHook.toString() : QString(); }
  void    setTriggerHook(const QString& value) { triggerHook = QJSValue(value); }
  QString getTriggerHook() const { return triggerHook.isString() ? triggerHook.toString() : QString(); }

private:
  QString  symbol;
  QString  localTranslationPath;
  QString  defaultNextState;
  QJSValue availableHook;
  QJSValue textHook;
  QJSValue triggerHook;
};


#endif
