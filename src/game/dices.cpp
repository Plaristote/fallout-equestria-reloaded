#include "dices.hpp"

int BalancedDiceRoller::random(int max)
{
  return m_generator->bounded(1, max + 1);
}

int BalancedDiceRoller::roll(int max)
{
  if (max <= 1) return 1;

  int result;
  QVector<int>& history = requireDiceHistoryFor(max);
  bool shouldManipulateChance = m_generator->generateDouble() < m_balancingChance;

  if (!shouldManipulateChance || history.size() < m_historySize / 4)
  {
    result = random(max);
  }
  else
  {
    const QVector<int>    segments = countSegments(history, max);
    const QVector<double> currentDistribution = calculateSegmentDistribution(segments);
    const QVector<int>    underrepresentedSegments = findUnderrepresentedSegments(currentDistribution);

    if (!underrepresentedSegments.isEmpty())
    {
      int segmentIndex = underrepresentedSegments[m_generator->bounded(underrepresentedSegments.size())];
      int segmentSize = max / m_segmentCount;
      int lowerBound = segmentIndex * segmentSize + 1;
      int upperBound = (segmentIndex + 1) * segmentSize;

      // Adjust last segment to include all the remaining values
      if (segmentIndex == m_segmentCount - 1)
        upperBound = max;

      // Generate a number within that segment
      result = m_generator->bounded(lowerBound, upperBound + 1);
    } else
    {
      result = random(max);
    }
  }
  history.append(result);
  if (history.size() > m_historySize)
    history.removeFirst();
  return result;
}

QVector<int>& BalancedDiceRoller::requireDiceHistoryFor(int max)
{
  if (!m_diceHistory.contains(max))
    m_diceHistory[max] = QVector<int>();
  return m_diceHistory[max];
}

QVector<int> BalancedDiceRoller::findUnderrepresentedSegments(const QVector<double>& currentDistribution) const
{
  double idealBinPercentage = 1.0 / m_segmentCount; // Uniform ideal distribution
  QVector<int> underrepresentedBins;

  for (int i = 0; i < m_segmentCount; ++i)
  {
    if (currentDistribution[i] < idealBinPercentage * 0.7)
      underrepresentedBins.append(i);
  }
  return underrepresentedBins;
}

QVector<int> BalancedDiceRoller::countSegments(const QVector<int>& history, int max) const
{
  QVector<int> segments(m_segmentCount, 0);
  int binSize = max / m_segmentCount;

  for (int value : history)
  {
    int segmentIndex = (value - 1) / binSize;
    if (segmentIndex >= m_segmentCount)
      segmentIndex = m_segmentCount - 1;
    segments[segmentIndex]++;
  }
  return segments;
}

QVector<double> BalancedDiceRoller::calculateSegmentDistribution(const QVector<int>& segments) const
{
  QVector<double> distribution(m_segmentCount, 0.0);
  int total = 0;

  for (int count : segments)
    total += count;
  if (total > 0)
  {
    for (int i = 0; i < m_segmentCount; ++i)
      distribution[i] = static_cast<double>(segments[i]) / total;
  }
  else
  {
    for (int i = 0; i < m_segmentCount; ++i)
      distribution[i] = 1.0 / m_segmentCount;
  }
  return distribution;
}
