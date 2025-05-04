#include "state.h"
#include "data.h"
#include "../characterdialog.h"
#include "utils/javascriptforeach.h"

QString jsErrorBacktrace(QJSValue retval);

DialogStateData::DialogStateData(DialogData& data) : data(data)
{
}

void DialogStateData::loadFromJavaScript(const QString& symbol, QJSValue value)
{
  this->symbol = symbol;
  triggerHook = value;
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
  object["answers"] = QJsonValue::fromVariant(QVariant::fromValue(defaultAnswers));
  if (mood.length() > 0)
    object["mood"] = mood;
  if (triggerHook.isString() && !triggerHook.toString().isEmpty())
    object["hook"] = triggerHook.toString();
}

DialogState DialogStateData::loadStateFromScriptObject(const CharacterDialog& dialog, DialogState state, QJSValue retval)
{
  QJSValue textKey = retval.property("textKey");
  QJSValue text = retval.property("text");
  QJSValue answers = retval.property("answers");
  QJSValue mood = retval.property("mood");

  if (!text.isUndefined())
    state.text = text.isCallable() ? text.call().toString() : text.toString();
  else if (textKey.isString())
    state.text = dialog.t(textKey.toString());
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

DialogState DialogStateData::setAsCurrentState(const CharacterDialog& dialog)
{
  DialogState state;
  QJSValue    retval;

  state.text = dialog.t(localTranslationPath);
  state.answers = defaultAnswers;
  state.mood = mood;
  if (triggerHook.isCallable())
  {
    retval = triggerHook.call();
    if (retval.isError())
      qDebug() << "DialogState: hook:" << jsErrorBacktrace(retval);
  }
  else if (triggerHook.isString() && dialog.getScript()->hasMethod(triggerHook.toString()))
    retval = dialog.getScript()->call(triggerHook.toString());
  else if (triggerHook.isObject())
    retval = triggerHook;
  if (retval.isString())
    state.text = retval.toString();
  else if (retval.isObject())
    return loadStateFromScriptObject(dialog, state, retval);
  return state;
}
