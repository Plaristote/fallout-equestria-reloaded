#ifndef  SCRIPTABLECOMPONENT_H
# define SCRIPTABLECOMPONENT_H

# include "../../scriptcontroller.h"
# include "utils/storableobject.h"
# include "globals.h"

class ScriptableComponent : public StorableObject
{
  Q_OBJECT
  typedef StorableObject ParentType;

  Q_PROPERTY(QString scriptName READ getScriptName NOTIFY scriptNameChanged)
  Q_PROPERTY(bool    hasScript  READ hasScript NOTIFY scriptNameChanged)
public:
  explicit ScriptableComponent(QObject *parent = nullptr);
  virtual ~ScriptableComponent();

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  Q_INVOKABLE virtual void setScript(const QString& name);
  QString getScriptName() const { return scriptName; }
  bool hasScript() const { return script != nullptr; }

  Q_INVOKABLE QJSValue  scriptCall(const QString& method, const QString& message = "") const;
  QJSValue              scriptCall(const QString& method, const QJSValueList& params) const;
  Q_INVOKABLE QJSValue  getScriptObject() const;
  QJSValue asJSValue();

signals:
  void scriptNameChanged();

protected slots:
  void initializeIfNeeded();

protected:
  virtual QString getScriptPath() const { return SCRIPTS_PATH + "behaviour"; }
  ScriptController* script = nullptr;
  QString scriptName;
  bool scriptInitialized = false;
};

#endif // SCRIPTABLECOMPONENT_H
