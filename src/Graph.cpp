#include "Graph.h"
#include "Config.h"
#include <cmath>
#include <iostream>
#include <random>

Graph::Graph() = default;

void Graph::addNode(int id, sf::Vector2f position) {
  m_nodes[id] = std::make_shared<Node>(id, position, NODE_CONFIG::BASE_RADIUS);
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

  // Initialize random number generators
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<int> nodeCountDist(
      ALGO_CONFIG::MIN_NODES, ALGO_CONFIG::MAX_NODES);
  static std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
  static std::uniform_real_distribution<float> radiusDist(
      GRAPH_CONFIG::getMinRadiusPlacement(),
      GRAPH_CONFIG::getMaxRadiusPlacement());
  static std::uniform_real_distribution<float> edgeProbDist(0.0f, 1.0f);
  static std::uniform_int_distribution<int> connectDist(1, 3);

  int targetNodes = nodeCountDist(gen);
  std::vector<sf::Vector2f> positions;

  if (GRAPH_CONFIG::DEBUG_PLACEMENT) {
    std::cout << "Attempting to place " << targetNodes << " nodes\n";
  }

  // Place nodes using hierarchical strategy
  for (int i = 0; i < targetNodes; ++i) {
    sf::Vector2f newPos;
    bool positionFound = false;

    if (i == 0) {
      // First node always goes at center - guaranteed valid
      newPos =
          sf::Vector2f(GRAPH_CONFIG::getCenterX(), GRAPH_CONFIG::getCenterY());
      positionFound = true;

      if (GRAPH_CONFIG::DEBUG_PLACEMENT) {
        std::cout << "Node " << i << ": Center position\n";
      }
    } else {
      // Try hierarchical placement strategies
      int totalAttempts = 0;

      // STRATEGY 1: Ring Placement (most preferred)
      for (int attempt = 0;
           attempt < GRAPH_CONFIG::RING_PLACEMENT_ATTEMPTS && !positionFound;
           ++attempt) {
        newPos = generateRingPosition(gen, angleDist, radiusDist, edgeProbDist);
        if (isValidPosition(newPos, positions)) {
          positionFound = true;
          if (GRAPH_CONFIG::DEBUG_PLACEMENT) {
            std::cout << "Node " << i << ": Ring placement (attempt "
                      << attempt + 1 << ")\n";
          }
        }
        totalAttempts++;
      }

      // STRATEGY 2: Grid Placement (fallback)
      for (int attempt = 0;
           attempt < GRAPH_CONFIG::GRID_PLACEMENT_ATTEMPTS && !positionFound;
           ++attempt) {
        newPos = generateGridPosition(i, targetNodes, edgeProbDist, gen);
        if (isValidPosition(newPos, positions)) {
          positionFound = true;
          if (GRAPH_CONFIG::DEBUG_PLACEMENT) {
            std::cout << "Node " << i << ": Grid placement (attempt "
                      << attempt + 1 << ")\n";
          }
        }
        totalAttempts++;
      }

      // STRATEGY 3: Random Placement (desperate)
      for (int attempt = 0;
           attempt < GRAPH_CONFIG::RANDOM_PLACEMENT_ATTEMPTS && !positionFound;
           ++attempt) {
        newPos = generateRandomPosition(gen);
        if (isValidPosition(newPos, positions)) {
          positionFound = true;
          if (GRAPH_CONFIG::DEBUG_PLACEMENT) {
            std::cout << "Node " << i << ": Random placement (attempt "
                      << attempt + 1 << ")\n";
          }
        }
        totalAttempts++;
      }

      if (GRAPH_CONFIG::DEBUG_PLACEMENT && totalAttempts > 50) {
        std::cout << "Node " << i << " required " << totalAttempts
                  << " attempts\n";
      }
    }

    // Add node if position found, otherwise reduce graph size
    if (positionFound) {
      positions.push_back(newPos);
      addNode(i, newPos);
    } else {
      if (GRAPH_CONFIG::DEBUG_PLACEMENT) {
        std::cout << "Failed to place node " << i << ". Reducing graph to " << i
                  << " nodes.\n";
      }
      targetNodes = i; // Reduce target to successfully placed nodes
      break;
    }
  }

  // Update numNodes to actual placed count
  int actualNodes = static_cast<int>(positions.size());

  if (GRAPH_CONFIG::DEBUG_PLACEMENT) {
    std::cout << "Successfully placed " << actualNodes << " nodes\n";
  }

  // === CONNECTIVITY PHASE (improved but same logic) ===

  // Step 1: Create spanning tree to ensure connectivity
  std::vector<bool> connected(actualNodes, false);
  connected[0] = true;

  for (int i = 1; i < actualNodes; ++i) {
    // Find closest connected node
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

  // Step 2: Add additional edges for interesting structure
  for (int i = 0; i < actualNodes; ++i) {
    int extraConnections = connectDist(gen);

    for (int attempt = 0; attempt < extraConnections; ++attempt) {
      for (int j = i + 1; j < actualNodes; ++j) {
        // Check if already connected
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

          // Distance-based probability with better scaling
          float maxConnectDistance =
              GRAPH_CONFIG::getMaxRadiusPlacement() * 2.0f;
          float probability =
              std::max(0.05f, 0.6f - (distance / maxConnectDistance));

          if (edgeProbDist(gen) < probability) {
            addEdge(i, j);
            break;
          }
        }
      }
    }
  }
}

sf::Vector2f Graph::generateRingPosition(
    std::mt19937 &gen, std::uniform_real_distribution<float> &angleDist,
    std::uniform_real_distribution<float> &radiusDist,
    std::uniform_real_distribution<float> &edgeProbDist) const {
  const float centerX = GRAPH_CONFIG::getCenterX();
  const float centerY = GRAPH_CONFIG::getCenterY();

  // Generate position in ring around center
  float angle = angleDist(gen);
  float radius = radiusDist(gen);

  sf::Vector2f newPos;
  newPos.x = centerX + radius * std::cos(angle);
  newPos.y = centerY + radius * std::sin(angle);

  // Add controlled random offset
  float offsetRange = GRAPH_CONFIG::getRandomOffsetRange();
  newPos.x += (edgeProbDist(gen) - 0.5f) * offsetRange;
  newPos.y += (edgeProbDist(gen) - 0.5f) * offsetRange;

  return clampToBounds(newPos);
}

sf::Vector2f
Graph::generateGridPosition(int nodeIndex, int totalNodes,
                            std::uniform_real_distribution<float> &edgeProbDist,
                            std::mt19937 &gen) const {
  // Calculate grid dimensions
  int gridSize = static_cast<int>(std::ceil(std::sqrt(totalNodes))) + 1;
  int gridX = (nodeIndex - 1) % gridSize; // -1 because node 0 is at center
  int gridY = (nodeIndex - 1) / gridSize;

  // Calculate grid position
  float spacing = GRAPH_CONFIG::getGridSpacing();
  sf::Vector2f newPos;
  newPos.x = GRAPH_CONFIG::getLeftBoundary() + (gridX + 1) * spacing;
  newPos.y = GRAPH_CONFIG::getTopBoundary() + (gridY + 1) * spacing;

  // Add small jitter to avoid perfect alignment
  float jitterRange = spacing * 0.2f; // 20% of spacing
  newPos.x += (edgeProbDist(gen) - 0.5f) * jitterRange;
  newPos.y += (edgeProbDist(gen) - 0.5f) * jitterRange;

  return clampToBounds(newPos);
}

sf::Vector2f Graph::generateRandomPosition(std::mt19937 &gen) const {
  std::uniform_real_distribution<float> xDist(GRAPH_CONFIG::getLeftBoundary(),
                                              GRAPH_CONFIG::getRightBoundary());
  std::uniform_real_distribution<float> yDist(
      GRAPH_CONFIG::getTopBoundary(), GRAPH_CONFIG::getBottomBoundary());

  return sf::Vector2f(xDist(gen), yDist(gen));
}

bool Graph::isValidPosition(
    const sf::Vector2f &newPos,
    const std::vector<sf::Vector2f> &existingPositions) const {
  const float minDistance = GRAPH_CONFIG::getSafeMinDistance();

  for (const auto &existingPos : existingPositions) {
    float distance = std::sqrt(std::pow(newPos.x - existingPos.x, 2) +
                               std::pow(newPos.y - existingPos.y, 2));
    if (distance < minDistance) {
      return false;
    }
  }
  return true;
}

sf::Vector2f Graph::clampToBounds(const sf::Vector2f &position) const {
  sf::Vector2f clamped = position;
  clamped.x = std::max(GRAPH_CONFIG::getLeftBoundary(),
                       std::min(GRAPH_CONFIG::getRightBoundary(), clamped.x));
  clamped.y = std::max(GRAPH_CONFIG::getTopBoundary(),
                       std::min(GRAPH_CONFIG::getBottomBoundary(), clamped.y));
  return clamped;
}
