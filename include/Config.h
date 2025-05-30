#pragma once
#include <SFML/Graphics.hpp>

class Config {
public:
  // Singleton pattern for global access
  static Config &getInstance() {
    static Config instance;
    return instance;
  }

  /* Window Config */
  struct Window {
    static constexpr unsigned int WIDTH = 1600;
    static constexpr unsigned int HEIGHT = 900;
    static constexpr float ASPECT_RATIO = static_cast<float>(WIDTH) / HEIGHT;
    static constexpr const char *TITLE = "BFS Visualizer";
  };

  /* Node Configuration */
  struct Node {
    static constexpr float BASE_RADIUS = 40.0f;
    static constexpr float OUTLINE_THICKNESS = 3.0f;
  };

  /* Graph Layout Configuration */
  struct Graph {
    // Ratios will be used to place graph relative to window's dimensions
    static constexpr float CENTER_X_RATIO = 0.65f; // 65% across window
    static constexpr float CENTER_Y_RATIO = 0.5f;  // 50% down window
    static constexpr float MIN_RADIUS_PLACEMENT_RATIO = 0.15f;
    static constexpr float MAX_RADIUS_PLACEMENT_RATIO = 0.22f;

    // Boundaries relative to window
    static constexpr float LEFT_BOUNDARY_RATIO = 0.35f;
    static constexpr float RIGHT_BOUNDARY_RATIO = 0.95f;
    static constexpr float TOP_BOUNDARY_RATIO = 0.15f;
    static constexpr float BOTTOM_BOUNDARY_RATIO = 0.85f;

    static constexpr float MIN_NODE_DISTANCE_MULTIPLIER = 3.0f;
    static constexpr float MIN_NODE_DISTANCE =
        Node::BASE_RADIUS * MIN_NODE_DISTANCE_MULTIPLIER;

    // Calculated absolute values
    static float getCenterX() { return Window::WIDTH * CENTER_X_RATIO; }
    static float getCenterY() { return Window::HEIGHT * CENTER_Y_RATIO; }
    static float getMinRadiusPlacement() {
      return Window::HEIGHT * MIN_RADIUS_PLACEMENT_RATIO;
    }
    static float getMaxRadiusPlacement() {
      return Window::HEIGHT * MAX_RADIUS_PLACEMENT_RATIO;
    }
    static float getLeftBoundary() {
      return Window::WIDTH * LEFT_BOUNDARY_RATIO;
    }
    static float getRightBoundary() {
      return Window::WIDTH * RIGHT_BOUNDARY_RATIO;
    }
    static float getTopBoundary() {
      return Window::HEIGHT * TOP_BOUNDARY_RATIO;
    }
    static float getBottomBoundary() {
      return Window::HEIGHT * BOTTOM_BOUNDARY_RATIO;
    }

    // PLACEMENT STRATEGIES
    static constexpr int RING_PLACEMENT_ATTEMPTS = 50;
    static constexpr int GRID_PLACEMENT_ATTEMPTS = 50;
    static constexpr int RANDOM_PLACEMENT_ATTEMPTS = 50;

    // IMPROVED SPACING CALCULATIONS
    static constexpr float RANDOM_OFFSET_MULTIPLIER = 0.3f;
    static constexpr float GRID_SPACING_MULTIPLIER = 1.5f;
    static constexpr float SAFETY_MARGIN = 1.1f;

    // CALCULATED HELPER FUNCTIONS
    static float getSafeMinDistance() {
      return MIN_NODE_DISTANCE * SAFETY_MARGIN;
    }

    static float getRandomOffsetRange() {
      return MIN_NODE_DISTANCE * RANDOM_OFFSET_MULTIPLIER;
    }

    static float getGridSpacing() {
      return MIN_NODE_DISTANCE * GRID_SPACING_MULTIPLIER;
    }

    // DEBUG SETTINGS
    static constexpr bool DEBUG_PLACEMENT = false; // Set to true for debugging
  };

  /* UI Configuration */
  struct UI {
    static constexpr unsigned int TITLE_FONT_SIZE = 64;
    static constexpr unsigned int NODE_FONT_SIZE = 48;

    // info box configuration
    static constexpr unsigned int INFO_FONT_SIZE = 32;
    static constexpr float LINE_HEIGHT = INFO_FONT_SIZE + 6;
    static constexpr float INFO_BOX_START_Y = 110.0f;

    static constexpr const char *QUEUE_BOX_TITLE = "QUEUE:";
    static constexpr float QUEUE_BOX_WIDTH = 50.0f;
    static constexpr float QUEUE_BOX_HEIGHT = 40.0f;
    static constexpr unsigned int QUEUE_BOX_FONT_SIZE = 32;
    static constexpr float UI_MARGIN = 20.0f;
  };

  /* Algorithm Configuration */
  struct Algorithm {
    static constexpr float DEFAULT_STEP_DELAY = 1.0f;
    static constexpr int MIN_NODES = 6;
    static constexpr int MAX_NODES = 12; // More nodes for larger window
    static constexpr int MAX_PLACEMENT_ATTEMPTS =
        150; // More attempts for denser layouts
  };

  /* Colors (could be made configurable later) */
  struct Colors {
    static constexpr sf::Color BACKGROUND = sf::Color::White;
    static constexpr sf::Color UNVISITED = sf::Color::White;
    static constexpr sf::Color IN_QUEUE = sf::Color::Yellow;
    static constexpr sf::Color CURRENT = sf::Color::Red;
    static constexpr sf::Color VISITED = sf::Color::Green;
    static constexpr sf::Color EDGE = sf::Color::Black;
    static constexpr sf::Color TEXT = sf::Color::Black;
    static constexpr sf::Color QUEUE_BOX = sf::Color::Yellow;
  };

private:
  Config() = default; // Singleton - private constructor
  ~Config() = default;
  Config(const Config &) = delete;
  Config &operator=(const Config &) = delete;
};

// Convenience macros for cleaner code (optional, but common in game dev)
#define WINDOW_CONFIG Config::Window
#define NODE_CONFIG Config::Node
#define GRAPH_CONFIG Config::Graph
#define UI_CONFIG Config::UI
#define ALGO_CONFIG Config::Algorithm
#define COLOR_CONFIG Config::Colors
