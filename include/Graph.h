#pragma once
#include "Node.h"
#include <SFML/Graphics.hpp>
#include <random>
#include <unordered_map>

class Graph {
public:
  Graph();
  ~Graph() = default;

  // Graph construction
  void addNode(int id, sf::Vector2f position);
  void addEdge(int from, int to);
  void clear();

  // Getters
  NodePtr getNode(int id) const;
  const std::unordered_map<int, NodePtr> &getNodes() const { return m_nodes; }

  // Interaction
  int getNodeAtPosition(sf::Vector2f position) const;

  // Rendering
  void draw(sf::RenderWindow &window, sf::Font &font) const;

  // Graph generation
  void generateSampleGraph();

private:
  std::unordered_map<int, NodePtr> m_nodes;

  void drawEdge(sf::RenderWindow &window, const Node &from,
                const Node &to) const;

  // PLACEMENT STRATEGY HELPERS
  sf::Vector2f generateRingPosition(
      std::mt19937 &gen, std::uniform_real_distribution<float> &angleDist,
      std::uniform_real_distribution<float> &radiusDist,
      std::uniform_real_distribution<float> &edgeProbDist) const;
  sf::Vector2f
  generateGridPosition(int nodeIndex, int totalNodes,
                       std::uniform_real_distribution<float> &edgeProbDist,
                       std::mt19937 &gen) const;
  sf::Vector2f generateRandomPosition(std::mt19937 &gen) const;
  bool
  isValidPosition(const sf::Vector2f &newPos,
                  const std::vector<sf::Vector2f> &existingPositions) const;
  sf::Vector2f clampToBounds(const sf::Vector2f &position) const;
};
