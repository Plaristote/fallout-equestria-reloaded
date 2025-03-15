#ifndef SCRIPTEDITORCONTROLLER_H
#define SCRIPTEDITORCONTROLLER_H

#include <QObject>
#include <QJSValue>
#include "cmap/statmodel.h"

class ScriptEditorController : public QObject
{
  Q_OBJECT
public:
  explicit ScriptEditorController(QObject *parent = nullptr);

  Q_INVOKABLE QStringList getScripts(const QString& type);
  Q_INVOKABLE QString     getScript(const QString& type, const QString& name);
  Q_INVOKABLE QString     setScript(const QString& type, const QString& name, const QString& content);

  Q_INVOKABLE QStringList getCharacterSheets();
  Q_INVOKABLE void        saveCharacterSheet(const QString& name, StatModel*);
  Q_INVOKABLE void        loadCharacterSheet(const QString& name, StatModel*);

  Q_INVOKABLE QStringList getLevels();
  Q_INVOKABLE QStringList getFactions();
  Q_INVOKABLE QStringList getDialogs();

  Q_INVOKABLE QStringList getFunctionsFromScript(QJSValue);

  Q_INVOKABLE void newDialog(const QString& name);

  Q_INVOKABLE void setTranslation(const QString& key, const QString& text, const QString& translationFile = "");

signals:
  void error(QString message);
};

#endif // SCRIPTEDITORCONTROLLER_H
