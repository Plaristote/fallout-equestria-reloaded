#include "levelgrid.h"
#include "tilemap/tilezone.h"
#include "../objects/doorway.h"
#include <cmath>

bool LevelGrid::CaseContent::isBlocked() const
{
  if (!occupied)
  {
    for (const auto* zone : zones)
    {
      if (zone->getAccessBlocked())
        return true;
    }
    return false;
  }
  return true;
}

bool LevelGrid::CaseContent::isLinkedTo(QPoint position) const
{
  auto iterator = std::find_if(connections.begin(), connections.end(), [this, position](const CaseConnection* connection) -> bool
  {
    auto* target = connection->getTargetFor(this);

    return target->position == position && !target->isBlocked();
  });
  return iterator != connections.end();
}

LevelGrid::CaseConnection* LevelGrid::CaseContent::connectionWith(CaseContent* other) const
{
  auto iterator = std::find_if(connections.begin(), connections.end(), [this, other](const CaseConnection* connection) -> bool
  {
    return connection->getTargetFor(this) == other;
  });

  return iterator != connections.end() ? *iterator : nullptr;
}

void LevelGrid::CaseContent::connectWith(CaseContent* other)
{
  if (other)
  {
    CaseConnection* connection = new CaseConnection;

    connection->connect(this, other);
  }
}

void LevelGrid::CaseContent::disconnectFrom(CaseContent* other)
{
  for (auto* connection : connections)
  {
    if (connection->getTargetFor(this) == other)
    {
      connection->disconnect();
      break ;
    }
  }
}

void LevelGrid::CaseContent::clearConnections()
{
  while (connections.size())
    (*connections.begin())->disconnect();
}

void LevelGrid::CaseConnection::connect(CaseContent* a, CaseContent* b)
{
  pair = {a, b};
  a->disconnectFrom(b);
  b->disconnectFrom(a);
  a->connections.push_back(this);
  b->connections.push_back(this);
}

template<typename ARRAY, typename VALUE>
void removeFrom(ARRAY& array, VALUE value)
{
  auto it = std::find(array.begin(), array.end(), value);

  if (it != array.end())
      array.erase(it);
}

void LevelGrid::CaseConnection::disconnect()
{
  removeFrom(pair.first->connections, this);
  removeFrom(pair.second->connections, this);
  delete this;
}

float LevelGrid::CaseConnection::getCost() const
{
  if (doorway && !doorway->property("opened").toBool())
    return 3.f;
  if (pair.first->position.z != pair.second->position.z)
    return 3.f;
  return 1.f;
}

bool LevelGrid::CaseConnection::canGoThrough(CharacterMovement* character)
{
  if (doorway)
    return doorway->canGoThrough(reinterpret_cast<Character*>(character));
  return true;
}

bool LevelGrid::CaseConnection::goThrough(CharacterMovement* character)
{
  if (doorway)
    return doorway->onGoThrough(reinterpret_cast<Character*>(character));
  return true;
}

std::list<LevelGrid::CaseContent*> LevelGrid::CaseContent::GetSuccessors(const CaseContent* parent, Actor* actor) const
{
  std::list<LevelGrid::CaseContent*> results;

  for (auto* connection : connections)
  {
    CaseContent* node = connection->getTargetFor(this);

    if ((!parent || node->position != parent->position) && !node->isBlocked() && connection->canGoThrough(actor))
      results.push_back(node);
  }
  return results;
}

float LevelGrid::CaseContent::GoalDistanceEstimate(const CaseContent& other) const
{
  int distX = position.x - other.position.x;
  int distY = position.y - other.position.y;
  int distFloor = std::abs(position.z - other.position.x);

  return std::sqrt(static_cast<float>(distX * distX + distY * distY)) + static_cast<float>(distFloor * 10);
}
