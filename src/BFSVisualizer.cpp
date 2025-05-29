#include "BFSVisualizer.h"
#include <sstream>

BFSVisualizer::BFSVisualizer(Graph &graph)
    : m_graph(graph), m_state(BFSState::READY), m_currentNode(-1),
      m_startNode(-1), m_autoStep(false), m_stepDelay(1.0f),
      m_timeSinceLastStep(0.0f) {}

void BFSVisualizer::startBFS(int startNodeId) {
  auto startNode = m_graph.getNode(startNodeId);
  if (!startNode)
    return;

  reset();
  m_startNode = startNodeId;
  m_state = BFSState::RUNNING;

  // Initialize BFS
  m_queue.push(startNodeId);
  m_visited.insert(startNodeId);
  m_visitOrder.push_back(startNodeId);

  updateNodeStates();
}

void BFSVisualizer::step() {
  if (m_state != BFSState::RUNNING)
    return;

  if (m_queue.empty()) {
    m_state = BFSState::FINISHED;
    m_currentNode = -1;
    updateNodeStates();
    return;
  }

  // Get next node from queue
  m_currentNode = m_queue.front();
  m_queue.pop();

  // Visit current node and add unvisited neighbors to queue
  auto currentNode = m_graph.getNode(m_currentNode);
  if (currentNode) {
    for (int neighborId : currentNode->getNeighbors()) {
      if (m_visited.find(neighborId) == m_visited.end()) {
        m_visited.insert(neighborId);
        m_queue.push(neighborId);
        m_visitOrder.push_back(neighborId);
      }
    }
  }

  updateNodeStates();

  // Check if BFS is complete
  if (m_queue.empty()) {
    m_state = BFSState::FINISHED;
    m_currentNode = -1;
    updateNodeStates();
  }
}

void BFSVisualizer::pause() {
  if (m_state == BFSState::RUNNING) {
    m_state = BFSState::PAUSED;
  }
}

void BFSVisualizer::resume() {
  if (m_state == BFSState::PAUSED) {
    m_state = BFSState::RUNNING;
  }
}

void BFSVisualizer::reset() {
  m_state = BFSState::READY;
  m_currentNode = -1;
  m_startNode = -1;

  // Clear BFS data structures
  while (!m_queue.empty())
    m_queue.pop();
  m_visited.clear();
  m_visitOrder.clear();

  resetNodeStates();
}

void BFSVisualizer::update(float deltaTime) {
  if (m_autoStep && m_state == BFSState::RUNNING) {
    m_timeSinceLastStep += deltaTime;
    if (m_timeSinceLastStep >= m_stepDelay) {
      step();
      m_timeSinceLastStep = 0.0f;
    }
  }
}

void BFSVisualizer::updateNodeStates() {
  // Only update states that have actually changed
  static std::unordered_set<int> lastVisited;
  static std::queue<int> lastQueue;
  static int lastCurrent = -1;

  // Reset all nodes to unvisited
  for (const auto &[id, node] : m_graph.getNodes()) {
    node->setState(NodeState::UNVISITED);
  }

  // Mark visited nodes
  for (int nodeId : m_visitOrder) {
    auto node = m_graph.getNode(nodeId);
    if (node) {
      node->setState(NodeState::VISITED);
    }
  }

  // Mark nodes in queue
  std::queue<int> tempQueue = m_queue;
  while (!tempQueue.empty()) {
    auto node = m_graph.getNode(tempQueue.front());
    if (node) {
      node->setState(NodeState::IN_QUEUE);
    }
    tempQueue.pop();
  }

  // Mark current node
  if (m_currentNode != -1) {
    auto node = m_graph.getNode(m_currentNode);
    if (node) {
      node->setState(NodeState::CURRENT);
    }
  }
}

void BFSVisualizer::resetNodeStates() {
  for (const auto &[id, node] : m_graph.getNodes()) {
    node->setState(NodeState::UNVISITED);
  }
}

void BFSVisualizer::drawQueue(sf::RenderWindow &window, sf::Font &font) const {
  const float startX = 20.0f;
  const float startY = 20.0f;
  const float boxWidth = 40.0f;
  const float boxHeight = 30.0f;

  // Title
  sf::Text title(font);
  title.setString("Queue:");
  title.setCharacterSize(16);
  title.setPosition({startX, startY});
  title.setFillColor(sf::Color::Black);
  window.draw(title);

  // Queue visualization
  std::queue<int> tempQueue = m_queue;
  float x = startX;
  float y = startY + 25.0f;

  while (!tempQueue.empty()) {
    // Draw box
    sf::RectangleShape box(sf::Vector2f(boxWidth, boxHeight));
    box.setPosition({x, y});
    box.setFillColor(sf::Color::Yellow);
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color::Black);
    window.draw(box);

    // Draw number
    sf::Text number(font);
    number.setString(std::to_string(tempQueue.front()));
    number.setCharacterSize(16);
    sf::FloatRect bounds = number.getLocalBounds();
    number.setPosition({x + boxWidth / 2.0f - bounds.size.x / 2.0f,
                        y + boxHeight / 2.0f - bounds.size.y / 2.0f - 2.0f});
    number.setFillColor(sf::Color::Black);
    window.draw(number);

    tempQueue.pop();
    x += boxWidth + 5.0f;
  }
}

void BFSVisualizer::drawInfo(sf::RenderWindow &window, sf::Font &font) const {
  const float startX = 20.0f;
  const float startY = 80.0f;
  const float lineHeight = 20.0f;

  std::vector<std::string> info;

  // State information
  std::string stateStr = "State: ";
  switch (m_state) {
  case BFSState::READY:
    stateStr += "Ready";
    break;
  case BFSState::RUNNING:
    stateStr += "Running";
    break;
  case BFSState::PAUSED:
    stateStr += "Paused";
    break;
  case BFSState::FINISHED:
    stateStr += "Finished";
    break;
  }
  info.push_back(stateStr);

  // Current node
  if (m_currentNode != -1) {
    info.push_back("Current Node: " + std::to_string(m_currentNode));
  }

  // Visit order
  if (!m_visitOrder.empty()) {
    std::ostringstream oss;
    oss << "Visit Order: ";
    for (size_t i = 0; i < m_visitOrder.size(); ++i) {
      if (i > 0)
        oss << " -> ";
      oss << m_visitOrder[i];
    }
    info.push_back(oss.str());
  }

  // Controls
  info.push_back("");
  info.push_back("Controls:");
  info.push_back("Space: Step / Start");
  info.push_back("R: Reset");
  info.push_back("A: Auto-step toggle");
  info.push_back("G: Generate new graph");
  info.push_back("Click node to start BFS");

  // Draw info
  for (size_t i = 0; i < info.size(); ++i) {
    sf::Text text(font);
    text.setString(info[i]);
    text.setCharacterSize(14);
    text.setPosition({startX, startY + i * lineHeight});
    text.setFillColor(sf::Color::Black);
    window.draw(text);
  }
}
