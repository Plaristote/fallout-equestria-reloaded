#include "characterdialogeditor.h"
#include <QJsonArray>

CharacterDialogEditor::CharacterDialogEditor(QObject *parent) : CharacterDialog(parent)
{
  connect(this, &CharacterDialog::ready, this, &CharacterDialogEditor::entryPointChanged);
  connect(this, &CharacterDialog::stateReferenceChanged, this, &CharacterDialogEditor::stateHookChanged);
  connect(this, &CharacterDialog::stateReferenceChanged, this, &CharacterDialogEditor::stateTextChanged);
  connect(this, &CharacterDialogEditor::currentOptionChanged, this, &CharacterDialogEditor::optionHookChanged);
  connect(this, &CharacterDialogEditor::currentOptionChanged, this, &CharacterDialogEditor::optionTextChanged);
  connect(this, &CharacterDialogEditor::currentOptionChanged, this, &CharacterDialogEditor::optionAvailableHookChanged);
  connect(this, &CharacterDialogEditor::currentOptionChanged, this, &CharacterDialogEditor::optionStateChanged);
}

void CharacterDialogEditor::save(const QString& name)
{
  QFile file(SCRIPTS_PATH + "/dialogs/" + name + ".json");

  if (file.open(QIODevice::WriteOnly))
  {
    file.write(QJsonDocument(data).toJson());
    file.close();
  }
}

void CharacterDialogEditor::setEntryPoint(const QString& value)
{
  data.insert("entryPoint", value);
  emit entryPointChanged();
}

void CharacterDialogEditor::setStateHook(const QString& value)
{
  QStringList answers;

  setStateVariable("hook", value);
  emit stateTextChanged();
  loadState(stateReference);
}

void CharacterDialogEditor::setStateText(const QString& value)
{
  QStringList answers;

  setStateVariable("text", value);
  emit stateTextChanged();
  initializeStateHook(text, answers);
  if (text.length() == 0)
    text = t(value);
  emit  textChanged();
}

void CharacterDialogEditor::setOptionHook(const QString& value)
{
  setOptionVariable("hook", value);
  emit optionHookChanged();
  emit optionsChanged();
}

void CharacterDialogEditor::setOptionState(const QString& value)
{
  setOptionVariable("state", value);
  emit optionStateChanged();
}

void CharacterDialogEditor::setOptionAvailableHook(const QString& value)
{
  setOptionVariable("availableHook", value);
  emit optionAvailableHookChanged();
  loadState(stateReference);
}

void CharacterDialogEditor::setOptionText(const QString& value)
{
  setOptionVariable("text", value);
  emit optionTextChanged();
  emit optionsChanged();
}

void CharacterDialogEditor::setOptionTextHook(const QString& value)
{
  setOptionVariable("textHook", value);
  emit optionTextHookChanged();
  emit optionsChanged();
}

QString CharacterDialogEditor::getStateVariable(const QString& name)
{
  QJsonObject stateData = data["states"].toObject()[stateReference].toObject();

  return stateData[name].toString();
}

void    CharacterDialogEditor::setStateVariable(const QString& name, const QString& value)
{
  QJsonObject states = data["states"].toObject();
  QJsonObject state  = states[stateReference].toObject();

  state.insert(name, value);
  states.insert(stateReference, state);
  data.insert("states", states);
}

QString CharacterDialogEditor::getOptionVariable(const QString& name)
{
  QJsonObject answerData = data["answers"].toObject()[currentOption].toObject();

  return answerData[name].toString();
}

void    CharacterDialogEditor::setOptionVariable(const QString& name, const QString& value)
{
  QJsonObject answers = data["answers"].toObject();
  QJsonObject answer  = answers[currentOption].toObject();

  answer.insert(name, value);
  answers.insert(currentOption, answer);
  data.insert("answers", answers);
}

void CharacterDialogEditor::newState(const QString &name)
{
  QJsonObject states = data["states"].toObject();
  QJsonObject state;

  state.insert("answers", QJsonArray());
  state.insert("text", "");
  states.insert(name, state);
  data.insert("states", states);
  loadState(name);
  emit stateListChanged();
}

void CharacterDialogEditor::newAnswer(const QString &name)
{
  QJsonObject states  = data["states"].toObject();
  QJsonObject answers = data["answers"].toObject();
  QJsonObject state   = states[stateReference].toObject();
  QJsonArray  stateAnswers = state["answers"].toArray();

  if (!answers[name].isObject())
  {
    QJsonObject answer;

    answers.insert(name, answer);
    data.insert("answers", answers);
  }
  if (!stateAnswers.contains(name))
  {
    stateAnswers.push_front(name);
    state.insert("answers", stateAnswers);
    states.insert(stateReference, state);
    data.insert("states", states);
    if (stateAnswers.size() == 1)
      options.clear();
    options.push_front(name);
  }
  currentOption = name;
  emit currentOptionChanged();
  emit optionsChanged();
}

void CharacterDialogEditor::removeState()
{
  QJsonObject states = data["states"].toObject();

  states.remove(stateReference);
  data.insert("states", states);
  emit stateListChanged();
}

void CharacterDialogEditor::removeAnswer()
{
  QJsonObject states  = data["states"].toObject();
  QJsonObject answers = data["answers"].toObject();
  QJsonObject state   = states[stateReference].toObject();
  QJsonArray  stateAnswers = state["answers"].toArray();

  for (auto it = stateAnswers.begin() ; it != stateAnswers.end() ; ++it)
  {
    if (it->toString() == currentOption)
    {
      stateAnswers.erase(it);
      break ;
    }
  }
  state.insert("answers", stateAnswers);
  states.insert(stateReference, state);
  data.insert("states", states);
  options.removeAll(currentOption);
  currentOption = "";
  emit currentOptionChanged();
  emit optionsChanged();
}

void CharacterDialogEditor::loadOption(const QString& value)
{
  options.push_back(value);
}
