#include "scripteditorcontroller.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>

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
    model->fromJson(QJsonObject());
}

QStringList ScriptEditorController::getLevels()
{
  QDir dir("assets/tilemaps");

  return dir.entryList(QStringList() << "*.json", QDir::NoFilter, QDir::Name);
}
