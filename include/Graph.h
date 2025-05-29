#pragma once
#include "Node.h"
#include <SFML/Graphics.hpp>
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
};
