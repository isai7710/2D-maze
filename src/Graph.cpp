#include "Graph.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

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

  // Initialize random number generator
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<int> nodeCountDist(6, 10);
  static std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
  static std::uniform_real_distribution<float> radiusDist(80.0f, 180.0f);
  static std::uniform_real_distribution<float> edgeProbDist(0.0f, 1.0f);
  static std::uniform_int_distribution<int> connectDist(1, 3);

  // Generate random number of nodes
  int numNodes = nodeCountDist(gen);
  const float centerX = 650.0f;
  const float centerY = 400.0f;
  const float minNodeDistance = 80.0f; // Minimum distance between nodes

  // Generate nodes with collision avoidance
  std::vector<sf::Vector2f> positions;

  for (int i = 0; i < numNodes; ++i) {
    sf::Vector2f newPos;
    int attempts = 0;
    bool validPosition = false;

    while (!validPosition && attempts < 100) {
      // Try circular/radial distribution for better spacing
      if (i == 0) {
        // First node at center
        newPos = sf::Vector2f(centerX, centerY);
      } else {
        // Generate position in a ring around center with some randomness
        float angle = angleDist(gen);
        float radius = radiusDist(gen);
        newPos.x = centerX + radius * std::cos(angle);
        newPos.y = centerY + radius * std::sin(angle);

        // Add some randomness to avoid perfect circles
        newPos.x += (edgeProbDist(gen) - 0.5f) * 60.0f;
        newPos.y += (edgeProbDist(gen) - 0.5f) * 60.0f;

        // Keep within bounds
        newPos.x = std::max(450.0f, std::min(850.0f, newPos.x));
        newPos.y = std::max(120.0f, std::min(680.0f, newPos.y));
      }

      // Check if position is valid (not too close to existing nodes)
      validPosition = true;
      for (const auto &existingPos : positions) {
        float distance = std::sqrt(std::pow(newPos.x - existingPos.x, 2) +
                                   std::pow(newPos.y - existingPos.y, 2));
        if (distance < minNodeDistance) {
          validPosition = false;
          break;
        }
      }
      attempts++;
    }

    positions.push_back(newPos);
    addNode(i, newPos);
  }

  // Create a connected graph using a better algorithm
  // Step 1: Create a spanning tree to ensure connectivity
  std::vector<bool> connected(numNodes, false);
  connected[0] = true;

  for (int i = 1; i < numNodes; ++i) {
    // Find closest connected node to current unconnected node
    int bestTarget = -1;
    float minDistance = std::numeric_limits<float>::max();

    for (int j = 0; j < i; ++j) {
      if (connected[j]) {
        float distance =
            std::sqrt(std::pow(positions[i].x - positions[j].x, 2) +
                      std::pow(positions[i].y - positions[j].y, 2));
        if (distance < minDistance) {
          minDistance = distance;
          bestTarget = j;
        }
      }
    }

    if (bestTarget != -1) {
      addEdge(i, bestTarget);
      connected[i] = true;
    }
  }

  // Step 2: Add additional random edges for more interesting structure
  for (int i = 0; i < numNodes; ++i) {
    // Each node gets 1-3 additional connection attempts
    int extraConnections = connectDist(gen);

    for (int attempt = 0; attempt < extraConnections; ++attempt) {
      for (int j = i + 1; j < numNodes; ++j) {
        // Skip if already connected
        auto nodeI = getNode(i);
        bool alreadyConnected = false;
        if (nodeI) {
          for (int neighbor : nodeI->getNeighbors()) {
            if (neighbor == j) {
              alreadyConnected = true;
              break;
            }
          }
        }

        if (!alreadyConnected) {
          float distance =
              std::sqrt(std::pow(positions[i].x - positions[j].x, 2) +
                        std::pow(positions[i].y - positions[j].y, 2));

          // Probability based on distance - closer nodes more likely to connect
          float probability = std::max(0.05f, 0.5f - (distance / 400.0f));

          if (edgeProbDist(gen) < probability) {
            addEdge(i, j);
            break; // Only add one extra connection per attempt
          }
        }
      }
    }
  }
}
