#include "characterdialogeditor.h"
#include <QJsonArray>

CharacterDialogEditor::CharacterDialogEditor(QObject *parent) : CharacterDialog(parent)
{
  connect(this, &CharacterDialog::ready, this, &CharacterDialogEditor::entryPointChanged);
  connect(this, &CharacterDialog::stateReferenceChanged, this, &CharacterDialogEditor::stateHookChanged);
  connect(this, &CharacterDialog::stateReferenceChanged, this, &CharacterDialogEditor::stateTextChanged);
  connect(this, &CharacterDialog::stateReferenceChanged, this, &CharacterDialogEditor::stateMoodChanged);
  connect(this, &CharacterDialogEditor::currentOptionChanged, this, &CharacterDialogEditor::optionHookChanged);
  connect(this, &CharacterDialogEditor::currentOptionChanged, this, &CharacterDialogEditor::optionTextChanged);
  connect(this, &CharacterDialogEditor::currentOptionChanged, this, &CharacterDialogEditor::optionTextHookChanged);
  connect(this, &CharacterDialogEditor::currentOptionChanged, this, &CharacterDialogEditor::optionAvailableHookChanged);
  connect(this, &CharacterDialogEditor::currentOptionChanged, this, &CharacterDialogEditor::optionStateChanged);
}

void CharacterDialogEditor::save(const QString& name)
{
  QFile file(SCRIPTS_PATH + "/dialogs/" + name + ".json");

  if (file.open(QIODevice::WriteOnly))
  {
    QJsonObject object;

    data.save(object);
    file.write(QJsonDocument(object).toJson());
    file.close();
  }
}

void CharacterDialogEditor::setEntryPoint(const QString& value)
{
  data.setDefaultEntryPoint(value);
  emit entryPointChanged();
}

void CharacterDialogEditor::setStateHook(const QString& value)
{
  QStringList answers;

  setStateVariable(&DialogStateData::setTriggerHook, value);
  emit stateTextChanged();
  loadState(stateReference);
}

void CharacterDialogEditor::setStateText(const QString& value)
{
  QStringList answers;

  setStateVariable(&DialogStateData::setLocalTranslationPath, value);
  emit stateTextChanged();
  if (text.length() == 0)
    text = t(value);
  emit textChanged();
}

void CharacterDialogEditor::setStateMood(const QString& value)
{
  setStateVariable(&DialogStateData::setMood, value);
  emit stateMoodChanged();
}

void CharacterDialogEditor::setOptionHook(const QString& value)
{
  setOptionVariable(&DialogAnswer::setTriggerHook, value);
  emit optionHookChanged();
  emit optionsChanged();
}

void CharacterDialogEditor::setOptionState(const QString& value)
{
  setOptionVariable(&DialogAnswer::setDefaultNextState, value);
  emit optionStateChanged();
}

void CharacterDialogEditor::setOptionAvailableHook(const QString& value)
{
  setOptionVariable(&DialogAnswer::setAvailableHook, value);
  emit optionAvailableHookChanged();
  loadState(stateReference);
}

void CharacterDialogEditor::setOptionText(const QString& value)
{
  setOptionVariable(&DialogAnswer::setLocalTranslationPath, value);
  emit optionTextChanged();
  emit optionsChanged();
}

void CharacterDialogEditor::setOptionTextHook(const QString& value)
{
  setOptionVariable(&DialogAnswer::setTextHook, value);
  emit optionTextHookChanged();
  emit optionsChanged();
}

QString CharacterDialogEditor::getStateVariable(DialogStateDataGetter getter)
{
  DialogStateData* state = data.findState(stateReference).get();

  if (state)
    return (state->*getter)();
  return QString();
}

void CharacterDialogEditor::setStateVariable(DialogStateDataSetter setter, const QString& value)
{
  DialogStateData* state = data.findState(stateReference).get();

  if (state)
    (state->*setter)(value);
}

QString CharacterDialogEditor::getOptionVariable(DialogAnswerGetter getter)
{
  DialogAnswer* answer = data.findAnswer(currentOption).get();

  if (answer)
    return (answer->*getter)();
  return QString();
}

void CharacterDialogEditor::setOptionVariable(DialogAnswerSetter setter, const QString& value)
{
  DialogAnswer* answer = data.findAnswer(currentOption).get();

  if (answer)
    (answer->*setter)(value);
}

void CharacterDialogEditor::newState(const QString &name)
{
  data.createNewState(name);
  loadState(name);
  emit stateListChanged();
}

void CharacterDialogEditor::newAnswer(const QString &name)
{
  const auto& currentState = data.findState(stateReference);

  if (currentState)
  {
    QStringList answerList = currentState->getDefaultAnswers();

    if (!answerList.contains(name))
    {
      data.createNewAnswer(name);
      answerList.push_front(name);
      currentState->setDefaultAnswers(answerList);
      if (answerList.size() == 1)
        options.clear();
      options.push_front(name);
    }
    currentOption = name;
    emit currentOptionChanged();
    emit optionsChanged();
  }
}

void CharacterDialogEditor::removeState()
{
  data.removeState(stateReference);
  emit stateListChanged();
}

void CharacterDialogEditor::removeAnswer()
{
  const auto& currentState = data.findState(stateReference);

  if (currentState)
  {
    QStringList answerList = currentState->getDefaultAnswers();

    answerList.removeOne(currentOption);
    currentState->setDefaultAnswers(answerList);
    data.removeAnswerIfUnreferenced(currentOption);
  }
  options.removeAll(currentOption);
  currentOption = "";
  emit currentOptionChanged();
  emit optionsChanged();
}

void CharacterDialogEditor::loadOption(const QString& value)
{
  options.push_back(value);
}

void CharacterDialogEditor::moveOptionUp(int index)
{
  const auto& state = data.findState(stateReference);
  QStringList answers = state->getDefaultAnswers();

  if (index > 0)
  {
    QString a = answers.at(index - 1);
    QString b = answers.at(index);

    answers[index - 1] = b;
    answers[index]     = a;
    state->setDefaultAnswers(answers);
    loadState(stateReference);
  }
}

void CharacterDialogEditor::moveOptionDown(int index)
{
  const auto& state = data.findState(stateReference);
  QStringList answers = state->getDefaultAnswers();

  if (index + 1 < answers.size())
  {
    QString a = answers.at(index + 1);
    QString b = answers.at(index);

    answers[index + 1] = b;
    answers[index]     = a;
    state->setDefaultAnswers(answers);
    loadState(stateReference);
  }
}
