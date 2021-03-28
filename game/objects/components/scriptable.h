#ifndef  SCRIPTABLECOMPONENT_H
# define SCRIPTABLECOMPONENT_H

# include "../../sprite.h"
# include "../../scriptcontroller.h"
# include "globals.h"

class ScriptableComponent : public Sprite
{
  Q_OBJECT
  typedef Sprite ParentType;

  Q_PROPERTY(QString scriptName READ getScriptName NOTIFY scriptNameChanged)
public:
  explicit ScriptableComponent(QObject *parent = nullptr);
  virtual ~ScriptableComponent();

  void load(const QJsonObject&);
  void save(QJsonObject&) const;

  Q_INVOKABLE virtual void setScript(const QString& name);
  QString getScriptName() const { return scriptName; }

  Q_INVOKABLE QJSValue  scriptCall(const QString& method, const QString& message = "");
  Q_INVOKABLE QJSValue  getScriptObject() const;
  QJSValue asJSValue();

signals:
  void scriptNameChanged();

protected:
  virtual QString getScriptPath() const { return SCRIPTS_PATH + "behaviour"; }
  ScriptController* script = nullptr;
  QString scriptName;
};

#endif // SCRIPTABLECOMPONENT_H
