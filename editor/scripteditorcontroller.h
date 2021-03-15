#ifndef SCRIPTEDITORCONTROLLER_H
#define SCRIPTEDITORCONTROLLER_H

#include <QObject>
#include "cmap/statmodel.h"

class ScriptEditorController : public QObject
{
  Q_OBJECT
public:
  explicit ScriptEditorController(QObject *parent = nullptr);

  Q_INVOKABLE QStringList getScripts(const QString& type);
  Q_INVOKABLE QString     getScript(const QString& type, const QString& name);
  Q_INVOKABLE void        setScript(const QString& type, const QString& name, const QString& content);

  Q_INVOKABLE QStringList getCharacterSheets();
  Q_INVOKABLE void        saveCharacterSheet(const QString& name, StatModel*);
  Q_INVOKABLE void        loadCharacterSheet(const QString& name, StatModel*);

  Q_INVOKABLE QStringList getLevels();
  Q_INVOKABLE QStringList getFactions();
  Q_INVOKABLE QStringList getDialogs();

  Q_INVOKABLE void newDialog(const QString& name);

  Q_INVOKABLE void setTranslation(const QString& key, const QString& text);

signals:

};

#endif // SCRIPTEDITORCONTROLLER_H
