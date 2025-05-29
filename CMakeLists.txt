cmake_minimum_required(VERSION 3.16)
project(BFS_Visualizer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SFML packages
find_package(SFML 3 COMPONENTS Window Graphics REQUIRED)

# Source files
set (SOURCES 
  main.cpp
  src/Node.cpp
  src/Graph.cpp
  src/BFSVisualizer.cpp
  src/Application.cpp
)

# Header files
set (HEADERS 
  include/Application.h 
  include/BFSVisualizer.h 
  include/Graph.h 
  include/Node.h
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
  SFML::Window
  SFML::Graphics
)

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Copy resources to build directory
# file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
