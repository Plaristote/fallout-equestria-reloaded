#ifndef  DICES_HPP
# define DICES_HPP

#include <stdlib.h>
#include <time.h>
#include <QObject>
#include <QVector>
#include <QMap>
#include <QRandomGenerator>
#include <QtGlobal>
#include <cmath>

struct Dices
{
  static void Initialize(void)
  {
    srand(time(0));
  }
  
  static int Throw(unsigned int max)
  {
    if (max != 0)
      return ((rand() % max) + 1);
    return max;
  }

  static bool Test(int successPercentage)
  {
    return (Throw(100) <= successPercentage);
  }
};

class BalancedDiceRoller : public QObject
{
  Q_OBJECT
public:
  explicit BalancedDiceRoller(QObject *parent = nullptr)
    : QObject(parent), m_generator(QRandomGenerator::global())
  {
  }

  Q_INVOKABLE int roll() { return roll(100); }
  Q_INVOKABLE int roll(int max);
  Q_INVOKABLE int random() { return random(100); }
  Q_INVOKABLE int random(int max);

private:
  QVector<int>&   requireDiceHistoryFor(int max);
  QVector<int>    countSegments(const QVector<int>& history, int max) const;
  QVector<double> calculateSegmentDistribution(const QVector<int>& bins) const;
  QVector<int>    findUnderrepresentedSegments(const QVector<double>& currentDistribution) const;

  int    m_segmentCount    = 10;
  int    m_historySize     = 50;
  double m_balancingChance = 0.7;
  QRandomGenerator*       m_generator;
  QMap<int, QVector<int>> m_diceHistory;
};

#endif
