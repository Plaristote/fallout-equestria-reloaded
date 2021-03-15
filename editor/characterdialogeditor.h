#ifndef CHARACTERDIALOGEDITOR_H
#define CHARACTERDIALOGEDITOR_H

#include <QObject>
#include "game/characterdialog.h"

class CharacterDialogEditor : public CharacterDialog
{
  Q_OBJECT

  Q_PROPERTY(QString stateHook READ getStateHook WRITE setStateHook NOTIFY stateHookChanged)
  Q_PROPERTY(QString stateText READ getStateText WRITE setStateText NOTIFY stateTextChanged)
  Q_PROPERTY(QString currentOption MEMBER currentOption NOTIFY currentOptionChanged)
  Q_PROPERTY(QString optionHook READ getOptionHook WRITE setOptionHook NOTIFY optionHookChanged)
  Q_PROPERTY(QString optionState READ getOptionState WRITE setOptionState NOTIFY optionStateChanged)
  Q_PROPERTY(QString optionText READ getOptionText WRITE setOptionText NOTIFY optionTextChanged)
  Q_PROPERTY(QString optionTextHook READ getOptionTextHook WRITE setOptionTextHook NOTIFY optionTextHookChanged)
  Q_PROPERTY(QString optionAvailableHook READ getOptionAvailableHook WRITE setOptionAvailableHook NOTIFY optionAvailableHookChanged)
public:
  explicit CharacterDialogEditor(QObject *parent = nullptr);

  Q_INVOKABLE void save(const QString&);

  QString getStateHook()           { return getStateVariable("hook"); }
  QString getStateText()           { return getStateVariable("text"); }
  QString getOptionHook()          { return getOptionVariable("hook"); }
  QString getOptionState()         { return getOptionVariable("state"); }
  QString getOptionAvailableHook() { return getOptionVariable("availableHook"); }
  QString getOptionText()          { return getOptionVariable("text"); }
  QString getOptionTextHook()      { return getOptionVariable("textHook"); }
  void setStateHook(const QString&);
  void setStateText(const QString&);
  void setStateTextHook(const QString&);
  void setOptionHook(const QString&);
  void setOptionState(const QString&);
  void setOptionAvailableHook(const QString&);
  void setOptionText(const QString&);
  void setOptionTextHook(const QString&);

  Q_INVOKABLE void newState(const QString& name);
  Q_INVOKABLE void newAnswer(const QString& name);
  Q_INVOKABLE void removeState();
  Q_INVOKABLE void removeAnswer();

signals:
  void stateHookChanged();
  void stateTextChanged();
  void currentOptionChanged();
  void optionHookChanged();
  void optionStateChanged();
  void optionAvailableHookChanged();
  void optionTextChanged();
  void optionTextHookChanged();

private:
  QString getStateVariable(const QString&);
  void    setStateVariable(const QString&, const QString&);
  QString getOptionVariable(const QString&);
  void    setOptionVariable(const QString&, const QString&);

  QString currentOption;
};

#endif // CHARACTERDIALOGEDITOR_H
