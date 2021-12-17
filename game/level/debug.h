#ifndef  DEBUGCOMPONENT_H
# define DEBUGCOMPONENT_H

# include "levelbase.h"
# include "metrics.h"

class DebugComponent : public LevelBase
{
  Q_OBJECT
  typedef LevelBase ParentType;

  Q_PROPERTY(bool debugMode MEMBER debugMode NOTIFY debugModeChanged)
public:
  explicit DebugComponent(QObject *parent = nullptr);

  Q_INVOKABLE QString metricsHtml() { return performanceMetrics.html();}
  Q_INVOKABLE void resetMetrics() { performanceMetrics.reset(); }

signals:
  void debugModeChanged();

protected:
  bool               debugMode = false;
  PerformanceReport  performanceMetrics;
};

#endif // DEBUGCOMPONENT_H
