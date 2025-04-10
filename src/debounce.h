#ifndef  MY_DEBOUNCER_H
# define MY_DEBOUNCER_H

# include <functional>
# include <QTimer>
# include <QSharedPointer>

template<typename INVOKABLE>
std::function<void()> createDebouncer(INVOKABLE callback, int delay)
{
  QSharedPointer<QTimer> timer(new QTimer);

  QObject::connect(timer.get(), &QTimer::timeout, callback);
  timer->setSingleShot(true);
  return [timer, delay]()
  {
    timer->stop();
    timer->start(delay);
  };
}

template<typename INVOKABLE>
std::function<void()> createFunnel(INVOKABLE callback, int delay)
{
  QSharedPointer<QTimer> timer(new QTimer);

  QObject::connect(timer.get(), &QTimer::timeout, callback);
  timer->setSingleShot(true);
  return [timer, delay]()
  {
    if (!timer->isActive())
      timer->start(delay);
  };
}

#endif
