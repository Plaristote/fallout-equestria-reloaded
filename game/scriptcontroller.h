#ifndef  SCRIPTCONTROLLER_H
# define SCRIPTCONTROLLER_H

# include <QJSEngine>
# include <QJSValue>

class ScriptController
{
public:
  ScriptController(const QString& modulePath, QObject* object);

  bool     hasMethod(const QString& method);
  QJSValue call(const QString& method, const QJSValueList& = QJSValueList());
  QJSValue property(const QString& name);

private:
  QJSEngine& engine;
  QString    path;
  QJSValue   module, instance;
};

#endif // SCRIPTCONTROLLER_H
