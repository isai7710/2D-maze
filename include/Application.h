#pragma once
#include "BFSVisualizer.h"
#include "Graph.h"
#include <SFML/Graphics.hpp>
#include <memory>

class Application {
public:
  Application();
  ~Application() = default;

  void run();

private:
  // Core components
  sf::RenderWindow m_window;
  sf::Font m_font;
  sf::Clock m_clock;

  // Application objects
  std::unique_ptr<Graph> m_graph;
  std::unique_ptr<BFSVisualizer> m_visualizer;

  // Event handling
  void handleEvents();
  void handleKeyPressed(sf::Keyboard::Key key);
  void handleMousePressed(sf::Vector2f mousePos);

  // Rendering
  void update(float deltaTime);
  void render();

  // Initialization
  bool loadResources();
  void initializeGraph();
};
