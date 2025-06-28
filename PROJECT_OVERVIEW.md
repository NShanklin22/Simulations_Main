# C++ Algorithm Simulation App - Project Overview

## Project Summary
A modular C++ application featuring algorithm visualizations with a Fallout terminal theme. The app provides an interactive main menu to select different algorithm simulations, starting with quicksort visualization.

## Technical Specifications

### Core Technologies
- **Language**: C++17
- **Graphics Library**: SFML (Simple and Fast Multimedia Library)
- **Build System**: CMake
- **Target Platform**: Windows (with cross-platform compatibility in mind)

### Key Features
- Main menu system for algorithm selection
- Fallout terminal-inspired theme (green/amber colors, monospace fonts)
- Real-time and step-by-step algorithm visualization
- User controls: pause, play, step-through, speed adjustment
- Configuration file system for persistent settings
- Modular architecture for easy algorithm additions

## Project Architecture

### Folder Structure
```
Simulations_Main/
├── src/                        # Source code
│   ├── main.cpp               # Application entry point
│   ├── core/                  # Core application systems
│   │   ├── Application.cpp    # Main app loop and window management
│   │   ├── MenuSystem.cpp     # Main menu and navigation
│   │   ├── ThemeManager.cpp   # Fallout theme implementation
│   │   └── ConfigManager.cpp  # Settings and config file handling
│   └── simulations/           # Algorithm simulations
│       └── sorting/           # Sorting algorithms category
│           └── quicksort/     # Quicksort implementation
│               ├── QuickSortVisualizer.cpp
│               └── QuickSortController.cpp
├── include/                   # Header files
│   ├── core/
│   └── simulations/
├── assets/                    # Resources
│   ├── fonts/                 # Terminal-style fonts
│   └── sounds/                # Audio files (placeholder for future)
├── config/                    # Configuration files
│   ├── settings.json          # User preferences
│   └── theme.json            # Theme configuration
├── docs/                      # Documentation
│   ├── cpp_explanations.md   # Detailed C++ concept explanations
│   └── algorithm_details.md  # Algorithm implementation notes
├── CMakeLists.txt            # Build configuration
└── README.md                 # Quick start guide
```

## Initial Algorithm: Quicksort Visualization

### Visualization Details
- **Display**: Bar chart representation of array elements
- **Array Size**: Configurable (default range: 10-100 elements)
- **Data Generation**: Random number generation
- **Animation**: Highlight compared elements, show pivot selection

### User Controls
- **Play/Pause**: Start/stop the animation
- **Step**: Advance one comparison when paused
- **Speed Control**: Adjustable delay between steps (50ms - 2000ms, default 500ms)
- **Reset**: Generate new random array
- **Array Size Slider**: Adjust number of elements

## Fallout Terminal Theme

### Visual Elements
- **Colors**: 
  - Primary: Bright green (#00FF41) for active text
  - Secondary: Amber (#FFA500) for highlights
  - Background: Dark green/black (#001100)
  - Inactive: Dim green (#006600)
- **Font**: Courier New or similar monospace terminal font
- **UI Style**: ASCII borders, terminal-like prompts
- **Sound Placeholders**: Framework ready for terminal beeps and typing sounds

## Configuration System

### Settings Categories
- **Display**: Window size, fullscreen toggle, theme variations
- **Simulation**: Default speed, array size preferences
- **Audio**: Volume levels (when implemented)
- **Controls**: Key bindings for pause/play/step

### File Format
JSON-based configuration for easy editing and version control.

## Development Phases

### Phase 1: Foundation (Current)
- Project structure setup
- Basic window and menu system
- Theme framework implementation

### Phase 2: Core Visualization
- Quicksort algorithm implementation
- Bar chart rendering system
- Basic user controls

### Phase 3: Enhancement
- Advanced controls and debugging features
- Configuration system integration
- Performance optimization

### Phase 4: Expansion Framework
- Plugin system for new algorithms
- Sound system integration
- Additional algorithm preparations

## Success Metrics
- Smooth 60fps visualization performance
- Intuitive user interface matching theme
- Modular code allowing easy algorithm additions
- Comprehensive documentation for learning C++ concepts
- Stable configuration system

## Future Algorithm Candidates
- **Sorting**: Bubble Sort, Merge Sort, Heap Sort
- **Pathfinding**: A*, Dijkstra, BFS/DFS
- **Data Structures**: Binary Tree traversals, Hash table operations
- **Search**: Binary Search, Linear Search variations