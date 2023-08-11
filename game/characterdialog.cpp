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

CharacterDialog::~CharacterDialog()
{
  if (script)
    delete script;
}

bool CharacterDialog::load(const QString& name, Character* player, DynamicObject* npc)
{
  QFile file(SCRIPTS_PATH + "/dialogs/" + name + ".json");

  if (file.open(QIODevice::ReadOnly))
  {
    this->player     = player;
    this->npc        = npc;
    translationGroup = "dialogs." + name;
    data.load(QJsonDocument::fromJson(file.readAll()).object());
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
  return data.getDefaultEntryPoint();
}

bool CharacterDialog::isOptionAvailable(const QString &answer)
{
  return data.isAnswerAvailable(*this, answer);
}

void CharacterDialog::loadOption(const QString &answer)
{
  if (isOptionAvailable(answer))
    options.push_back(answer);
}

void CharacterDialog::loadState(const QString& reference)
{
  const auto& stateData = data.findState(reference);
  DialogState state;

  if (stateData)
  {
    options.clear();
    if (!stateReference.isEmpty())
      stateHistory.push_front(stateReference);
    stateReference = reference;
    state   = stateData->setAsCurrentState(*this);
    text    = state.text;
    if (!state.mood.isEmpty())
      mood  = state.mood;
    for (const QString& answer : state.answers)
      loadOption(answer);
    if (state.answers.length() == 0)
      options.push_back("exit");
    emit stateReferenceChanged();
    emit textChanged();
    emit optionsChanged();
    emit moodChanged();
  }
  else
    emit dialogEnded();
}

void CharacterDialog::selectOption(const QString& answerSymbol)
{
  QString nextState;

  if (answerSymbol != "exit")
    nextState = getNextState(answerSymbol);
  if (!nextState.isEmpty())
    loadState(nextState);
  else
    emit dialogEnded();
}

QString CharacterDialog::getNextState(const QString& answerSymbol)
{
  const auto& answer = data.findAnswer(answerSymbol);

  if (answer)
    return answer->trigger(*this);
  return QString();
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
  const auto& answer = data.findAnswer(key);

  if (answer)
    return answer->getText(*this);
  return "[...]";
}

QStringList CharacterDialog::getStateList() const
{
  QStringList result;

  for (const auto& state : data.getStates())
    result.push_back(state->getSymbol());
  return result;
}

QStringList CharacterDialog::getAnswerList() const
{
  QStringList result;

  for (const auto& answer : data.getAnswers())
    result.push_back(answer->getSymbol());
  return result;
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
