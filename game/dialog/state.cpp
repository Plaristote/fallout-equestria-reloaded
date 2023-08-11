#include "state.h"
#include "data.h"
#include "../characterdialog.h"
#include "utils/javascriptforeach.h"

DialogStateData::DialogStateData(DialogData& data) : data(data)
{
}

void DialogStateData::load(const QString& symbol, const QJsonObject& object)
{
  this->symbol = symbol;
  localTranslationPath = object["text"].toString();
  mood = object["mood"].toString();
  defaultAnswers = object["answers"].toVariant().toStringList();
  if (object["hook"].isString())
    triggerHook = object["hook"].toString();
}

void DialogStateData::save(QJsonObject& object) const
{
  object["text"] = localTranslationPath;
  object["mood"] = mood;
  object["answers"] = QJsonValue::fromVariant(QVariant::fromValue(defaultAnswers));
  if (triggerHook.isString())
    object["hook"] = triggerHook.toString();
}

DialogState DialogStateData::loadStateFromScriptObject(DialogState state, QJSValue retval)
{
  QJSValue text = retval.property("text");
  QJSValue answers = retval.property("answers");
  QJSValue mood = retval.property("mood");

  if (!text.isUndefined())
    state.text = text.isCallable() ? text.call().toString() : text.toString();
  if (mood.isString())
    state.mood = mood.toString();
  if (answers.isArray())
  {
    state.answers.clear();
    forEach(answers, [this, &state](QJSValue answer)
    {
      if (answer.isString())
        state.answers.push_back(answer.toString());
      else if (answer.isObject())
        state.answers.push_back(data.registerAnswer(answer));
    });
  }
  return state;
}

DialogState DialogStateData::setAsCurrentState(CharacterDialog& dialog)
{
  DialogState state;
  QJSValue    retval;

  state.text = dialog.t(localTranslationPath);
  state.answers = defaultAnswers;
  state.mood = mood;
  if (triggerHook.isCallable())
    retval = triggerHook.call();
  else if (triggerHook.isString() && dialog.getScript()->hasMethod(triggerHook.toString()))
    retval = dialog.getScript()->call(triggerHook.toString());
  if (retval.isString())
    state.text = retval.toString();
  else if (retval.isObject())
    return loadStateFromScriptObject(state, retval);
  return state;
}
