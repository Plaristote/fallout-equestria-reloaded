#include "globals.h"
#include "characterdialog.h"
#include "game.h"
#include <QFile>

CharacterDialog::CharacterDialog(QObject *parent) : QObject(parent)
{
  mood     = "neutral";
  ambiance = "wasteland";
}

void CharacterDialog::load(const QString& name, Character* player, Character* npc)
{
  QFile file(SCRIPTS_PATH + "/dialogs/" + name + ".json");

  if (file.open(QIODevice::ReadOnly))
  {
    QJSValue initializer;

    this->player  = player;
    this->npc     = npc;
    self          = Game::get()->getScriptEngine().newQObject(this);
    data          = QJsonDocument::fromJson(file.readAll());
    script        = Game::get()->loadScript(SCRIPTS_PATH + "dialogs/" + name + ".mjs");
    initializer   = script.property("initialize");
    if (initializer.isCallable())
    {
      QJSValueList args;

      args << self;
      stateReference = initializer.call(args).toString();
    }
    else
      stateReference = data["entryPoint"].toString();
    loadState(stateReference);
  }
  else
    qDebug() << "Could not load dialog file " << name;
}

void CharacterDialog::loadState(const QString& reference)
{
  QJsonObject stateData = data[reference].toObject();
  QJsonObject optionsData = stateData["options"].toObject();

  options.clear();
  stateReference = reference;
  text = stateData["text"].toString();
  for (auto optionKey : optionsData.keys())
  {
    QJsonObject optionData = optionsData[optionKey].toObject();
    QJsonValue availableHook = optionData["availableHook"];

    qDebug() << "Checking dialog option" << optionKey << optionData["availableHook"];
    if (availableHook.isUndefined() || availableHook.isNull())
      options.push_back(optionKey);
    else
    {
      QJSValue availableCallback = script.property(availableHook.toString());

      if (availableCallback.isCallable() && availableCallback.call().toBool())
        options.push_back(optionKey);
      else
        qDebug() << "Not available du to hook";
    }
  }
  emit stateReferenceChanged();
  emit textChanged();
  emit optionsChanged();
  qDebug() << "Loaded dialog ztate" << reference << ": optionz are " << options;
  qDebug() << QJsonDocument(stateData).toJson();
}

void CharacterDialog::selectOption(const QString& key)
{
  QJsonObject optionData    = data[stateReference]["options"][key].toObject();
  QJsonValue  defaultAnswer = optionData["defaultAnswer"];
  QJsonValue  callbackName  = optionData["executeHook"];

  if (!(callbackName.isUndefined() || callbackName.isNull()))
  {
    QJSValue callback = script.property(callbackName.toString());

    if (callback.isCallable())
    {
      QString answer = callback.call().toString();
      if (!answer.isEmpty())
        loadState(answer);
      else
        emit dialogEnded();
    }
    else
      qDebug() << "Broken executeHook in dialog for" << stateReference << ":" << key;
  }
  else if (!defaultAnswer.isUndefined() && !defaultAnswer.toString().isEmpty())
    loadState(defaultAnswer.toString());
  else
    emit dialogEnded();
}

QString CharacterDialog::getOptionText(const QString& key)
{
  return data[stateReference]["options"][key]["text"].toString();
}
