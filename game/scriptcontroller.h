#ifndef  SCRIPTCONTROLLER_H
# define SCRIPTCONTROLLER_H

# include <QJSEngine>
# include <QJSValue>

class ScriptController
{
public:
  ScriptController(const QString& modulePath);

  void     initialize(QObject* object);
  bool     hasMethod(const QString& method);
  QJSValue call(const QString& method, const QJSValueList& = QJSValueList());
  QJSValue property(const QString& name);

  static QJSValue callFunction(QJSValue function, const QJSValueList& = QJSValueList());

  QJSValue getObject() const { return instance; }
  QJSValue getModel() const { return model; }

private:
  QJSEngine& engine;
  QString    path;
  QJSValue   module, instance, model;
};

#endif // SCRIPTCONTROLLER_H
