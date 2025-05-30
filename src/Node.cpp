#include "Node.h"
#include "Config.h"

Node::Node(int id, sf::Vector2f position, float radius)
    : m_id(id), m_position(position),
      m_radius(radius > 0 ? radius : NODE_CONFIG::BASE_RADIUS),
      m_state(NodeState::UNVISITED) {}

void Node::addNeighbor(int neighborId) {
  // Avoid duplicate neighbors
  for (int id : m_neighbors) {
    if (id == neighborId)
      return;
  }
  m_neighbors.push_back(neighborId);
}

void Node::draw(sf::RenderWindow &window, sf::Font &font) const {
  // Draw circle
  sf::CircleShape circle(m_radius);
  circle.setPosition({m_position.x - m_radius, m_position.y - m_radius});
  circle.setFillColor(getStateColor());
  circle.setOutlineThickness(NODE_CONFIG::OUTLINE_THICKNESS);
  circle.setOutlineColor(COLOR_CONFIG::EDGE);
  window.draw(circle);

  // Draw node ID
  sf::Text text(font);
  text.setString(std::to_string(m_id));
  text.setCharacterSize(UI_CONFIG::NODE_FONT_SIZE);
  sf::FloatRect textBounds = text.getLocalBounds();
  text.setPosition({m_position.x - textBounds.size.x / 2.0f,
                    m_position.y - textBounds.size.y / 2.0f - 12.0f});
  text.setFillColor(COLOR_CONFIG::TEXT);
  window.draw(text);
}

bool Node::contains(sf::Vector2f point) const {
  float dx = point.x - m_position.x;
  float dy = point.y - m_position.y;
  return (dx * dx + dy * dy) <= (m_radius * m_radius);
}

sf::Color Node::getStateColor() const {
  switch (m_state) {
  case NodeState::UNVISITED:
    return COLOR_CONFIG::UNVISITED;
  case NodeState::IN_QUEUE:
    return COLOR_CONFIG::IN_QUEUE;
  case NodeState::CURRENT:
    return COLOR_CONFIG::CURRENT;
  case NodeState::VISITED:
    return COLOR_CONFIG::VISITED;
  default:
    return COLOR_CONFIG::UNVISITED;
  }
}
