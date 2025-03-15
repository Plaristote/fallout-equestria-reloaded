#include "scripteditorcontroller.h"
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJSValueIterator>
#include "game.h"
#include "i18n.h"

const QRegularExpression jsExtensionExp("\\.m?js$");
const QMap<QString, QPair<QString, QString> > scriptDefaultSuper = {
  {"pnjs",  {"CharacterBehaviour", "character.mjs"}},
  {"items", {"ItemBehaviour", "item.mjs"}}
};

static QStringList recursively_find_files(const QString& path, QStringList extensions)
{
  QDir dir(path);
  QDirIterator it(dir.path(), extensions, QDir::Files, QDirIterator::Subdirectories);
  QStringList output;

  while (it.hasNext())
    output << it.next().replace(dir.path() + '/', "");
  output.sort();
  return output;
}

ScriptEditorController::ScriptEditorController(QObject *parent) : QObject(parent)
{
  connect(this, &ScriptEditorController::error, this, [](QString value) { qDebug() << "ScriptEditorController:" << value; });
}

QStringList ScriptEditorController::getScripts(const QString& type)
{
  return recursively_find_files("scripts/" + type, {"*.mjs","*.js"});
}

QStringList ScriptEditorController::getCharacterSheets()
{
  return recursively_find_files("assets/characterSheets", {"*.json"});
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
    qDebug() << "Could not open script: scripts/" << type << '/' << name;
  return result;
}

static QString capitalize(const QString& source)
{
  QString result;

  result.reserve(source.size());
  for (int i = 0 ; i < source.size() ; ++i)
  {
    if (i == 0 || !source[i - 1].isLetter())
      result += source[i].toUpper();
    else if (source[i].isLetter())
      result += source[i];
  }
  return result;
}

static QString scriptTemplate(const QString& scriptName, const QString& scriptCategory, const QString& relativePath)
{
  QString className = capitalize(scriptName);
  QString code;
  QPair<QString, QString> superclass;
  int depth = 1 + relativePath.split('/').size();

  if (scriptDefaultSuper.contains(scriptCategory))
    superclass = scriptDefaultSuper[scriptCategory];
  if (!superclass.first.isEmpty())
  {
    code += "import {" + superclass.first + "} from \"./";
    for (int i = 1 ; i < depth ; ++i)
      code += "../";
    code += superclass.second + "\";\n\n";
  }
  code += "class " + className;
  if (!superclass.first.isEmpty())
    code += " extends " + superclass.first;
  code += " {\n";
  code += "  constructor(model) {\n";
  if (!superclass.first.isEmpty())
    code += "    super(model);\n";
  else
    code += "    this.model = model;\n";
  code += "  }\n";
  code += "}\n\n";
  code += "export function create(model) {\n";
  code += "  return new " + className + "(model);\n";
  code += "}";
  return code;
}

static bool editFile(QFile& file, const QString& content)
{
  if (file.open(QIODevice::WriteOnly))
  {
    file.write(QByteArray(content.toStdString().c_str(), content.length()));
    file.close();
    return true;
  }
  return false;
}

QString ScriptEditorController::setScript(const QString &type, const QString &name, const QString &content)
{
  QString path = "scripts/" + type + '/';
  auto    relativePath = name.split('/');
  QString fullName(name + (name.indexOf(jsExtensionExp) >= 0 ? "" : ".mjs"));
  QFile   file(path + fullName);
  QString scriptName = relativePath.last();
  QDir    dir;

  scriptName.replace(jsExtensionExp, "");
  relativePath.removeLast();
  if (dir.mkpath(path + relativePath.join('/')))
  {
    bool result = true;

    if (!file.exists() && content.size() == 0)
      result = editFile(file, scriptTemplate(scriptName, type, relativePath.join('/')));
    else if (content.size() > 0)
      result = editFile(file, content);
    if (!result)
      emit error("Failed to open file " + path + name);
  }
  else
    emit error("Could not create directory " + path);
  return fullName;
}

void ScriptEditorController::saveCharacterSheet(const QString &name, StatModel* model)
{
  QFile file("assets/characterSheets/" + name);

  QDir().mkpath(QFileInfo(file).absoluteDir().path());
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

QStringList ScriptEditorController::getDialogs() { return recursively_find_files("scripts/dialogs", {"*.json"}); }

void ScriptEditorController::newDialog(const QString &name)
{
  QFile jsonFile(SCRIPTS_PATH + "dialogs/" + name + ".json");
  QFile mjsFile (SCRIPTS_PATH + "dialogs/" + name + ".mjs");

  QDir().mkpath(QFileInfo(jsonFile).absoluteDir().path());
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

void ScriptEditorController::setTranslation(const QString &key, const QString &text, const QString &translationFile)
{
  QString     locale = I18n::get()->getCurrentLocale();
  QString     sourcePath = translationFile.length() ? I18n::getSourceForLocale(translationFile, locale) : I18n::getSourceForLocale(locale);
  QFile       file(sourcePath);
  QJsonObject data;
  std::function<void(QJsonObject&, QStringList)> recursive;
  QStringList parts = key.split('.');

  if (!file.exists())
  {
    QDir        dir;
    QStringList pathParts = sourcePath.split('/');

    pathParts.removeLast();
    dir.mkpath(pathParts.join('/'));
    qDebug() << "setTranslation: created directory: " << pathParts.join('/');
  }
  else if (file.open(QIODevice::ReadOnly))
  {
    data = QJsonDocument::fromJson(file.readAll()).object();
    file.close();
  }
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
    I18n::get()->loadCurrentLocale();
  }
}

QStringList ScriptEditorController::getFunctionsFromScript(QJSValue script)
{
  QJSValueIterator it(script);
  QStringList functionList;

  qDebug() << "Looking for functions in script" << script.toString();
  while (it.hasNext())
  {
    if (it.value().isCallable() && script.hasOwnProperty(it.name()))
      functionList.append(it.name());
  }
  return functionList;
}
