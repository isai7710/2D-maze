#include "Graph.h"
#include <cmath>

Graph::Graph() = default;

void Graph::addNode(int id, sf::Vector2f position) {
  m_nodes[id] = std::make_shared<Node>(id, position);
}

void Graph::addEdge(int from, int to) {
  auto fromNode = getNode(from);
  auto toNode = getNode(to);

  if (fromNode && toNode) {
    fromNode->addNeighbor(to);
    toNode->addNeighbor(from); // Undirected graph
  }
}

void Graph::clear() { m_nodes.clear(); }

NodePtr Graph::getNode(int id) const {
  auto it = m_nodes.find(id);
  return (it != m_nodes.end()) ? it->second : nullptr;
}

int Graph::getNodeAtPosition(sf::Vector2f position) const {
  for (const auto &[id, node] : m_nodes) {
    if (node->contains(position)) {
      return id;
    }
  }
  return -1; // No node found
}

void Graph::draw(sf::RenderWindow &window, sf::Font &font) const {
  // Draw edges first (so they appear behind nodes)
  for (const auto &[id, node] : m_nodes) {
    for (int neighborId : node->getNeighbors()) {
      auto neighbor = getNode(neighborId);
      if (neighbor && id < neighborId) { // Avoid drawing edge twice
        drawEdge(window, *node, *neighbor);
      }
    }
  }

  // Draw nodes
  for (const auto &[id, node] : m_nodes) {
    node->draw(window, font);
  }
}

void Graph::drawEdge(sf::RenderWindow &window, const Node &from,
                     const Node &to) const {
  sf::Vector2f fromPos = from.getPosition();
  sf::Vector2f toPos = to.getPosition();

  // Calculate direction vector
  sf::Vector2f direction = toPos - fromPos;
  float length =
      std::sqrt(direction.x * direction.x + direction.y * direction.y);

  if (length > 0) {
    direction /= length;

    // Adjust positions to start/end at circle edges
    sf::Vector2f startPos = fromPos + direction * 25.0f;
    sf::Vector2f endPos = toPos - direction * 25.0f;

    // Create line using rectangle
    sf::RectangleShape line;
    line.setSize(sf::Vector2f(length - 50.0f, 2.0f));
    line.setPosition(startPos);
    line.setFillColor(sf::Color::Black);

    // Calculate rotation angle
    float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159f;
    line.setRotation(sf::degrees(angle));

    window.draw(line);
  }
}

void Graph::generateSampleGraph() {
  clear();

  // Create a sample graph with interesting structure
  addNode(0, sf::Vector2f(150, 150));
  addNode(1, sf::Vector2f(300, 100));
  addNode(2, sf::Vector2f(450, 150));
  addNode(3, sf::Vector2f(100, 300));
  addNode(4, sf::Vector2f(250, 250));
  addNode(5, sf::Vector2f(400, 300));
  addNode(6, sf::Vector2f(550, 250));
  addNode(7, sf::Vector2f(200, 400));
  addNode(8, sf::Vector2f(350, 450));

  // Add edges to create an interesting graph structure
  addEdge(0, 1);
  addEdge(0, 3);
  addEdge(1, 2);
  addEdge(1, 4);
  addEdge(2, 6);
  addEdge(3, 4);
  addEdge(3, 7);
  addEdge(4, 5);
  addEdge(4, 7);
  addEdge(4, 8);
  addEdge(5, 6);
  addEdge(5, 8);
  addEdge(7, 8);
}
