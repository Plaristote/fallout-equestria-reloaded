#include "data.h"

void DialogData::load(const QJsonObject& object)
{
  QJsonObject stateList  = object["states"].toObject();
  QJsonObject answerList = object["answers"].toObject();

  reset();
  for (const QString& symbol : stateList.keys())
  {
    QSharedPointer<DialogStateData> state(new DialogStateData(*this));

    state->load(symbol, stateList[symbol].toObject());
    states.push_back(state);
  }
  for (const QString& symbol : answerList.keys())
  {
    QSharedPointer<DialogAnswer> answer(new DialogAnswer);

    answer->load(symbol, answerList[symbol].toObject());
    answers.push_back(answer);
  }
  defaultEntryPoint = object["entryPoint"].toString();
}

void DialogData::save(QJsonObject& object) const
{
  QJsonObject stateList;
  QJsonObject answerList;

  for (const auto& state : states)
  {
    QJsonObject stateData;

    state->save(stateData);
    stateList.insert(state->getSymbol(), stateData);
  }
  for (const auto& answer : answers)
  {
    QJsonObject answerData;

    answer->save(answerData);
    answerList.insert(answer->getSymbol(), answerData);
  }
  object.insert("states", stateList);
  object.insert("answers", answerList);
  object.insert("entryPoint", defaultEntryPoint);
}

void DialogData::reset()
{
  states.clear();
  answers.clear();
}

QSharedPointer<DialogAnswer> DialogData::findAnswer(const QString& symbol)
{
  for (unsigned int i = 0 ; i < answers.count() ; ++i)
  {
    const auto& candidate = answers.at(i);

    if (candidate->getSymbol() == symbol)
      return candidate;
  }
  return nullptr;
}

QSharedPointer<DialogStateData> DialogData::findState(const QString& symbol)
{
  for (unsigned int i = 0 ; i < states.count() ; ++i)
  {
    const auto& candidate = states.at(i);

    if (candidate->getSymbol() == symbol)
      return candidate;
  }
  return nullptr;
}

QString DialogData::getAnswerText(CharacterDialog& dialog, const QString& symbol)
{
  auto answer = findAnswer(symbol);

  if (answer)
    return answer->getText(dialog);
  return QString();
}

bool DialogData::isAnswerAvailable(CharacterDialog& dialog, const QString& symbol)
{
  auto answer = findAnswer(symbol);

  return answer && answer->isAvailable(dialog);
}

QString DialogData::createStateFromAnswer(QJSValue data)
{
  QString uid = QUuid::createUuid().toString();
  DialogStateData& stateOnTheFly = createNewState(uid);

  stateOnTheFly.loadFromJavaScript(uid, data);
  return stateOnTheFly.getSymbol();
}

QString DialogData::triggerAnswer(CharacterDialog& dialog, const QString& symbol)
{
  auto answer = findAnswer(symbol);

  if (answer)
  {
    QJSValue retval(answer->trigger(dialog));

    if (retval.isObject())
      return createStateFromAnswer(retval);
    else if (retval.isString())
      return retval.toString();
  }
  return QString();
}

QString DialogData::registerAnswer(const QJSValue& object)
{
  QString symbol = object.property("symbol").toString();
  QSharedPointer<DialogAnswer> answer = findAnswer(symbol);

  if (!answer)
  {
    answer = QSharedPointer<DialogAnswer>(new DialogAnswer);
    answers.push_back(answer);
  }
  answer->load(object);
  return symbol;
}

DialogStateData& DialogData::createNewState(const QString& symbol)
{
  QSharedPointer<DialogStateData> state = findState(symbol);

  if (!state)
  {
    state = QSharedPointer<DialogStateData>(new DialogStateData(*this));
    state->setSymbol(symbol);
    state->setLocalTranslationPath(symbol);
    states.push_back(state);
  }
  return *state;
}

DialogAnswer& DialogData::createNewAnswer(const QString& symbol)
{
  QSharedPointer<DialogAnswer> answer = findAnswer(symbol);

  if (!answer)
  {
    answer = QSharedPointer<DialogAnswer>(new DialogAnswer);
    answer->setSymbol(symbol);
    answer->setLocalTranslationPath(symbol);
    answers.push_front(answer);
  }
  return *answer;
}

void DialogData::removeState(const QString& symbol)
{
  QSharedPointer<DialogStateData> state = findState(symbol);

  states.removeOne(state);
}

void DialogData::removeAnswerIfUnreferenced(const QString& symbol)
{
  QSharedPointer<DialogAnswer> answer = findAnswer(symbol);

  for (const auto& state : states)
  {
    if (state->getDefaultAnswers().contains(symbol))
      return ;
  }
  answers.removeOne(answer);
}
