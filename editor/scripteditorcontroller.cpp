#include "scripteditorcontroller.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include "game.h"
#include "i18n.h"

ScriptEditorController::ScriptEditorController(QObject *parent) : QObject(parent)
{
}

QStringList ScriptEditorController::getScripts(const QString& type)
{
  QDir dir("scripts/" + type);

  return dir.entryList(QStringList() << "*.mjs" << "*.js", QDir::NoFilter, QDir::Name);
}

QStringList ScriptEditorController::getCharacterSheets()
{
  QDir dir("assets/characterSheets");

  return dir.entryList(QStringList() << "*.json", QDir::NoFilter, QDir::Name);
}

QString ScriptEditorController::getScript(const QString &type, const QString &name)
{
  QFile file("scripts/" + type + '/' + name);
  QString result;

  if (file.open(QIODevice::ReadOnly))
  {
    result = file.readAll();
    file.close();
  }
  else
    qDebug() << "Could not open script: scripts7" << type << '/' << name;
  return result;
}

void ScriptEditorController::setScript(const QString &type, const QString &name, const QString &content)
{
  QFile file("scripts/" + type + '/' + name);

  if (file.open(QIODevice::WriteOnly))
  {
    file.write(QByteArray(content.toStdString().c_str(), content.length()));
    file.close();
  }
}

void ScriptEditorController::saveCharacterSheet(const QString &name, StatModel* model)
{
  QFile file("assets/characterSheets/" + name);

  if (file.open(QIODevice::WriteOnly))
  {
      QJsonObject data;

      model->toJson(data);
      file.write(QJsonDocument(data).toJson());
      file.close();
  }
}

void ScriptEditorController::loadCharacterSheet(const QString &name, StatModel* model)
{
  QFile file("assets/characterSheets/" + name);

  if (file.open(QIODevice::ReadOnly))
  {
    model->fromJson(QJsonDocument::fromJson(file.readAll()).object());
    file.close();
  }
  else
  {
    model->fromJson(QJsonObject());
    model->setProperty("specialPoints", 5);
    model->setHitPoints(model->get_maxHitPoints());
  }
}

QStringList ScriptEditorController::getLevels()
{
  QDir dir("assets/tilemaps");
  auto levels = dir.entryList(QStringList() << "*.json", QDir::NoFilter, QDir::Name);

  for (auto& levelName : levels)
    levelName.replace(".json", "");
  return levels;
}

QStringList ScriptEditorController::getFactions()
{
  Game* game = Game::get();

  if (game)
    return QStringList() << "" << game->getDataEngine()->getFactionList();
  return QStringList();
}

QStringList ScriptEditorController::getDialogs()
{
  QDir dir("scripts/dialogs");

  return dir.entryList(QStringList() << "*.json", QDir::NoFilter, QDir::Name);
}

void ScriptEditorController::newDialog(const QString &name)
{
  QFile jsonFile(SCRIPTS_PATH + "dialogs/" + name + ".json");
  QFile mjsFile (SCRIPTS_PATH + "dialogs/" + name + ".mjs");

  if (!jsonFile.exists() && jsonFile.open(QIODevice::WriteOnly))
  {
    QByteArray jsonText;

    jsonText = "{\"states\":{},\"answers\":{}}";
    jsonFile.write(jsonText);
    jsonFile.close();
    if (!mjsFile.exists() && mjsFile.open(QIODevice::WriteOnly))
    {
      QByteArray jsTemplate;

      jsTemplate = QByteArray("class Dialog {\n") +
                   "  constructor(dialog) {\n" +
                   "    this.dialog = dialog;\n" +
                   "  }\n" +
                   "}\n\n" +
                   "export function create(dialog) {\n" +
                   "  return new Dialog(dialog);\n" +
                   "}";
      mjsFile.write(jsTemplate);
      mjsFile.close();
    }
  }
}

void ScriptEditorController::setTranslation(const QString &key, const QString &text)
{
  QString locale = I18n::get()->getCurrentLocale();
  QFile   file(I18n::getSourceForLocale(locale));

  if (file.open(QIODevice::ReadOnly))
  {
    qDebug() << "UPDATING TRANsLATION FILE, key=" << key << ", text" << text;
    QJsonObject data = QJsonDocument::fromJson(file.readAll()).object();
    QStringList parts = key.split('.');
    std::function<void(QJsonObject&, QStringList)> recursive;

    file.close();
    recursive = [&recursive, text](QJsonObject& parent, QStringList remainingParts)
    {
      QString& key = remainingParts.first();

      if (remainingParts.length() > 1) {
        QJsonObject scope = parent[key].toObject();

        remainingParts.removeFirst();
        recursive(scope, remainingParts);
        parent.insert(key, scope);
      }
      else
        parent.insert(key, text);
    };
    recursive(data, parts);
    if (file.open(QIODevice::WriteOnly))
    {
      file.write(QJsonDocument(data).toJson());
      file.close();
      I18n::get()->loadLocale();
    }
  }
}
