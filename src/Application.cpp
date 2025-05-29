#include "Application.h"
#include <iostream>

Application::Application()
    : m_window(sf::VideoMode({1000, 800}), "BFS Visualizer",
               sf::Style::Titlebar | sf::Style::Close) {
  m_window.setFramerateLimit(60);

  if (!loadResources()) {
    throw std::runtime_error("Failed to load resources");
  }

  initializeGraph();
}

void Application::run() {
  while (m_window.isOpen()) {
    float deltaTime = m_clock.restart().asSeconds();

    handleEvents();
    update(deltaTime);
    render();
  }
}

bool Application::loadResources() {
  // Try to load a default font
  if (!m_font.openFromFile("../public/ShareTech-Regular.ttf")) {
    std::cerr
        << "Warning: Could not load font. Text may not display correctly.\n";
    // SFML will use a default font, so we can continue
  }
  return true;
}

void Application::initializeGraph() {
  m_graph = std::make_unique<Graph>();
  m_visualizer = std::make_unique<BFSVisualizer>(*m_graph);

  // Generate initial sample graph
  m_graph->generateSampleGraph();
}

void Application::handleEvents() {
  while (const auto event = m_window.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      m_window.close();
    }

    else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
      handleKeyPressed(keyPressed->code);
    }

    else if (const auto *mouseButtonPressed =
                 event->getIf<sf::Event::MouseButtonPressed>()) {
      if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
        sf::Vector2f mousePos(
            static_cast<float>(mouseButtonPressed->position.x),
            static_cast<float>(mouseButtonPressed->position.y));
        handleMousePressed(mousePos);
      }
    }
  }
}

void Application::handleKeyPressed(sf::Keyboard::Key key) {
  switch (key) {
  case sf::Keyboard::Key::Space:
    if (m_visualizer->getState() == BFSState::READY) {
      // Need to select a start node first
      std::cout << "Click on a node to start BFS\n";
    } else if (m_visualizer->getState() == BFSState::RUNNING) {
      m_visualizer->step();
    } else if (m_visualizer->getState() == BFSState::PAUSED) {
      m_visualizer->resume();
    }
    break;

  case sf::Keyboard::Key::R:
    m_visualizer->reset();
    break;

  case sf::Keyboard::Key::A:
    m_visualizer->setAutoStep(!m_visualizer->isAutoStepping());
    break;

  case sf::Keyboard::Key::G:
    m_visualizer->reset();
    m_graph->generateSampleGraph();
    break;

  case sf::Keyboard::Key::P:
    if (m_visualizer->getState() == BFSState::RUNNING) {
      m_visualizer->pause();
    } else if (m_visualizer->getState() == BFSState::PAUSED) {
      m_visualizer->resume();
    }
    break;

  default:
    break;
  }
}

void Application::handleMousePressed(sf::Vector2f mousePos) {
  int nodeId = m_graph->getNodeAtPosition(mousePos);
  if (nodeId != -1) {
    if (m_visualizer->getState() == BFSState::READY) {
      m_visualizer->startBFS(nodeId);
      std::cout << "Started BFS from node " << nodeId << "\n";
    }
  }
}

void Application::update(float deltaTime) { m_visualizer->update(deltaTime); }

void Application::render() {
  m_window.clear(sf::Color::White);

  // Draw graph
  m_graph->draw(m_window, m_font);

  // Draw BFS visualization elements
  m_visualizer->drawQueue(m_window, m_font);
  m_visualizer->drawInfo(m_window, m_font);

  m_window.display();
}
