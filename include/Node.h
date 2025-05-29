#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

enum class NodeState { UNVISITED, VISITED, CURRENT, IN_QUEUE };

class Node {
public:
  Node(int id, sf::Vector2f position, float radius = 25.0f);

  // Getters
  int getId() const { return m_id; }
  sf::Vector2f getPosition() const { return m_position; }
  NodeState getState() const { return m_state; }
  const std::vector<int> &getNeighbors() const { return m_neighbors; }

  // Setters
  void setState(NodeState state) { m_state = state; }
  void addNeighbor(int neighborId);

  // Rendering
  void draw(sf::RenderWindow &window, sf::Font &font) const;

  // Utility
  bool contains(sf::Vector2f point) const;

private:
  int m_id;
  sf::Vector2f m_position;
  float m_radius;
  NodeState m_state;
  std::vector<int> m_neighbors;

  sf::Color getStateColor() const;
};

using NodePtr = std::shared_ptr<Node>;
