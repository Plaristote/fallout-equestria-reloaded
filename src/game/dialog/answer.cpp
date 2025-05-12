#include "answer.h"
#include "../characterdialog.h"

QString jsErrorBacktrace(QJSValue retval);

void DialogAnswer::load(const QString& symbol, const QJsonObject& data)
{
  this->symbol = symbol;
  availableHook = textHook = triggerHook = QJSValue();
  localTranslationPath = data["text"].toString();
  defaultNextState = data["state"].toString();
  if (data.contains("availableHook"))
    availableHook = QJSValue(data["availableHook"].toString());
  if (data.contains("textHook"))
    textHook = QJSValue(data["textHook"].toString());
  if (data.contains("hook"))
    triggerHook = QJSValue(data["hook"].toString());
}

void DialogAnswer::load(const QJSValue& data)
{
  symbol = data.property("symbol").toString();
  if (data.hasProperty("text"))
    localTranslationPath = data.property("text").toString();
  if (data.hasProperty("state"))
    defaultNextState = data.property("state").toString();
  availableHook = data.property("availableHook");
  textHook = data.property("textHook");
  triggerHook = data.property("hook");
}

void DialogAnswer::save(QJsonObject& data) const
{
  if (!localTranslationPath.isEmpty())
    data["text"] = localTranslationPath;
  if (!defaultNextState.isEmpty())
    data["state"] = defaultNextState;
  if (availableHook.isString() && !availableHook.toString().isEmpty())
    data["availableHook"] = availableHook.toString();
  if (textHook.isString() && !textHook.toString().isEmpty())
    data["textHook"] = textHook.toString();
  if (triggerHook.isString() && !triggerHook.toString().isEmpty())
    data["hook"] = triggerHook.toString();
}

QString DialogAnswer::getText(CharacterDialog& dialog)
{
  QString callback("getAnswerText");

  if (textHook.isCallable())
  {
    QJSValue retval = textHook.call(QJSValueList() << dialog.getScriptObject()).toString();

    if (retval.isError())
      qDebug() << "DialogAsnwer: textHook:" << jsErrorBacktrace(retval);
    return retval.toString();
  }
  else if (textHook.isString())
    callback = textHook.toString();
  if (dialog.getScript()->hasMethod(callback))
  {
    QJSValue retval = dialog.getScript()->call(callback, QJSValueList() << symbol);

    if (retval.isString())
      return retval.toString();
  }
  if (localTranslationPath == "" || localTranslationPath == "exit")
    return "[...]";
  return dialog.t(localTranslationPath);
}

QJSValue DialogAnswer::trigger(CharacterDialog& dialog)
{
  ScriptController* script = dialog.getScript();
  QJSValue retval;

  if (triggerHook.isCallable())
  {
    retval = triggerHook.call(QJSValueList() << dialog.getScriptObject());
    if (retval.isError())
      qDebug() << "DialogAnswer: hook:" << jsErrorBacktrace(retval);
  }
  else if (triggerHook.isString() && script->hasMethod(triggerHook.toString()))
    retval = script->call(triggerHook.toString());
  if (retval.isString() || retval.isObject())
    return retval;
  return defaultNextState;
}

bool DialogAnswer::isAvailable(CharacterDialog& dialog)
{
  QJSValueList params{symbol};

  if (availableHook.isCallable())
  {
    QJSValue retval = availableHook.call(params);

    if (retval.isError())
      qDebug() << "DialogAnswer: availableHook: " << jsErrorBacktrace(retval);
    return retval.toBool();
  }
  else if (availableHook.isString() && !availableHook.toString().isEmpty())
  {
    QString  callback = availableHook.toString();
    QJSValue property;
    bool     inverse = callback.first(1) == '!';
    bool     result = true;

    if (inverse)
      callback = callback.slice(1);
    property = dialog.getScript()->property(callback);
    if (property.isCallable())
      result = dialog.getScript()->callMethod(property, params).toBool();
    else if (property.isBool())
      result = property.toBool();
    return inverse ? !result : result;
  }
  else if (availableHook.isBool())
    return availableHook.toBool();
  return true;
}
