#pragma once
#include "Graph.h"
#include <SFML/Graphics.hpp>
#include <queue>
#include <unordered_set>
#include <vector>

enum class BFSState { READY, RUNNING, PAUSED, FINISHED };

class BFSVisualizer {
public:
  BFSVisualizer(Graph &graph);
  ~BFSVisualizer() = default;

  // BFS Control
  void startBFS(int startNodeId);
  void step();
  void pause();
  void resume();
  void reset();

  // Getters
  BFSState getState() const { return m_state; }
  const std::queue<int> &getQueue() const { return m_queue; }
  const std::vector<int> &getVisitOrder() const { return m_visitOrder; }
  int getCurrentNode() const { return m_currentNode; }

  // Auto-stepping
  void setAutoStep(bool enable) { m_autoStep = enable; }
  void setStepDelay(float delay) { m_stepDelay = delay; }

  // Update (for auto-stepping)
  void update(float deltaTime);

  // Rendering
  void drawQueue(sf::RenderWindow &window, sf::Font &font) const;
  void drawInfo(sf::RenderWindow &window, sf::Font &font) const;

private:
  Graph &m_graph;
  BFSState m_state;

  // BFS data structures
  std::queue<int> m_queue;
  std::unordered_set<int> m_visited;
  std::vector<int> m_visitOrder;
  int m_currentNode;
  int m_startNode;

  // Auto-stepping
  bool m_autoStep;
  float m_stepDelay;
  float m_timeSinceLastStep;

  // Helper methods
  void updateNodeStates();
  void resetNodeStates();
};
