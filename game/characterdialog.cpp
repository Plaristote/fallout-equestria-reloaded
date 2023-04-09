#include "globals.h"
#include "characterdialog.h"
#include "game.h"
#include "utils/javascript2i18n.hpp"
#include <QFile>
#include <QJsonArray>
#include <QDebug>

CharacterDialog::CharacterDialog(QObject *parent) : QObject(parent)
{
  barter   = new BarterController(this);
  mood     = "neutral";
  ambiance = "wasteland";
  connect(this, &CharacterDialog::barterEnded, this, &CharacterDialog::onBarterEnded);
}

bool CharacterDialog::load(const QString& name, Character* player, DynamicObject* npc)
{
  QFile file(SCRIPTS_PATH + "/dialogs/" + name + ".json");

  if (file.open(QIODevice::ReadOnly))
  {
    this->player     = player;
    this->npc        = npc;
    translationGroup = "dialogs." + name;
    data             = QJsonDocument::fromJson(file.readAll()).object();
    script = new ScriptController(SCRIPTS_PATH + "dialogs/" + name + ".mjs");
    if (npc->isCharacter())
      barter->initialize(script, player, reinterpret_cast<Character*>(npc));
    connect(npc, &DynamicObject::objectNameChanged, this, &CharacterDialog::nameChanged);
    script->initialize(this);
    loadState(getEntryPoint());
    emit ready();
    return true;
  }
  else
    qDebug() << "Could not load dialog file " << (SCRIPTS_PATH + "/dialogs/" + name + ".json");
  return false;
}

QString CharacterDialog::getEntryPoint()
{
  if (script->hasMethod("getEntryPoint"))
  {
    auto entryPoint = script->call("getEntryPoint");

    if (entryPoint.isString())
      return entryPoint.toString();
  }
  return data["entryPoint"].toString();
}

void CharacterDialog::initializeStateHook(QString& text, QStringList& answers)
{
  QJsonObject statesData  = data["states"].toObject();
  QJsonObject stateData   = statesData[stateReference].toObject();
  QString triggerCallback = stateData["hook"].toString();

  text = "";
  if (script->hasMethod(triggerCallback))
  {
    QJSValue value = script->call(triggerCallback);

    if (value.isString())
      text = value.toString();
    else if (value.isObject())
    {
      if (value.hasProperty("text"))
        text = value.property("text").toString();
      if (value.hasProperty("answers"))
        answers = value.property("answers").toVariant().toStringList();
    }
  }
}

bool CharacterDialog::isOptionAvailable(const QString &answer)
{
  QJsonObject answerData = data["answers"].toObject();
  QJsonObject optionData = answerData[answer].toObject();
  QJsonValue  availableHook = optionData["availableHook"];
  QJSValue    retval;

  if (availableHook.isUndefined() || !script->hasMethod(availableHook.toString()))
    return true;
  retval = script->call(availableHook.toString());
  return retval.toBool();
}

void CharacterDialog::loadOption(const QString &answer)
{
  if (isOptionAvailable(answer))
    options.push_back(answer);
}

void CharacterDialog::loadState(const QString& reference)
{
  QJsonObject statesData  = data["states"].toObject();
  QJsonObject answerData  = data["answers"].toObject();
  QJsonObject stateData   = statesData[reference].toObject();
  QStringList answers;

  options.clear();
  stateReference = reference;
  if (stateData["mood"].toString().length() > 0)
    setProperty("mood", stateData["mood"].toString());
  initializeStateHook(text, answers);
  if (text.length() == 0)
    text = t(stateData["text"].toString());
  if (answers.length() == 0)
    answers = stateData["answers"].toVariant().toStringList();
  for (const QString& answer : answers)
    loadOption(answer);
  if (answers.length() == 0)
    loadOption("exit");
  emit stateReferenceChanged();
  emit textChanged();
  emit optionsChanged();
  qDebug() << "Loaded dialog ztate" << reference << ": optionz are " << options;
  qDebug() << QJsonDocument(stateData).toJson();
}

void CharacterDialog::selectOption(const QString& key)
{
  QString nextState;

  if (key != "exit")
    nextState = getNextState(key);
  if (nextState != "")
    loadState(nextState);
  else
    emit dialogEnded();
}

QString CharacterDialog::getNextState(const QString& answer)
{
  QJsonObject answersData  = data["answers"].toObject();
  QJsonObject optionData   = answersData[answer].toObject();
  QString     callbackName = optionData["hook"].toString();

  if (callbackName != "" && script->hasMethod(callbackName))
  {
    QJSValue retval = script->call(callbackName);

    if (retval.isString())
      return retval.toString();
  }
  return optionData["state"].toString();
}

QString CharacterDialog::getName() const
{
  return npc->getDisplayName();
}

QString CharacterDialog::t(const QString &name, const QVariantMap& options)
{
  QString translationPath = translationGroup + '.' + name;
  QVariantMap parameters{
    {"name", player->getStatistics()->getName()},
    {"npc", npc->isCharacter() ? reinterpret_cast<Character*>(npc)->getStatistics()->getName() : getName()}
  };

  parameters = fetchScriptVariablesFromTranslation(translationPath, parameters, script);
  for (auto it = options.begin() ; it != options.end() ; ++it)
    parameters.insert(it.key(), it.value());
  return I18n::get()->t(translationPath, parameters);
}

QString CharacterDialog::getOptionText(const QString& key)
{
  QJsonObject answers    = data["answers"].toObject();
  QJsonObject answerData = answers[key].toObject();
  QString     textHook   = answerData["textHook"].toString();
  QString     callback("getAnswerText");

  if (textHook.length() > 0)
    callback = textHook;
  if (script->hasMethod(callback))
  {
    QJSValue retval = script->call(callback, QJSValueList() << key);

    if (retval.isString())
      return retval.toString();
  }
  if (answerData["text"].toString() == "")
    return "[...]";
  return t(answerData["text"].toString());
}

QStringList CharacterDialog::getStateList() const
{
  return data["states"].toObject().keys();
}

QStringList CharacterDialog::getAnswerList() const
{
  return data["answers"].toObject().keys();
}

bool CharacterDialog::tryToBarter()
{
  if (npc->isCharacter() && barter->tryToBarter())
  {
    emit barterStarted();
    return true;
  }
  return false;
}

void CharacterDialog::onBarterEnded()
{
  if (script->hasMethod("onBarterEnded"))
    script->call("onBarterEnded");
}
