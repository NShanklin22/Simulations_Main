# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

### Prerequisites Setup
```bash
# Install SFML via vcpkg (Windows)
cd C:\vcpkg
.\vcpkg install sfml:x64-windows
```

### Build Commands
```bash
# Configure project
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_BUILD_TYPE=Release

# Build project
cmake --build . --config Release

# Run application
.\Release\SimulationApp.exe
```

### Development Build Types
- **Release**: `-DCMAKE_BUILD_TYPE=Release` (optimized)
- **Debug**: `-DCMAKE_BUILD_TYPE=Debug` (with debug symbols)
- **RelWithDebInfo**: `-DCMAKE_BUILD_TYPE=RelWithDebInfo` (optimized + debug symbols)

## Project Architecture

This is a C++17 SFML-based algorithm visualization application with a modular architecture designed around a Fallout terminal theme.

### Core Architecture Components

**Application Layer (`src/core/`)**:
- `Application` class manages the main game loop, window, and core systems
- `MenuSystem` handles navigation between different algorithm simulations
- `ThemeManager` applies the Fallout terminal aesthetic (green/amber colors, monospace fonts)
- `ConfigManager` handles JSON-based persistent settings

**Simulation Layer (`src/simulations/`)**:
- Organized by algorithm category (e.g., `sorting/`, future: `pathfinding/`, `search/`)
- Each algorithm has a `Visualizer` (rendering) and `Controller` (logic) class
- Currently implements: Quicksort with bar chart visualization

**Configuration System**:
- JSON-based settings in `config/settings.json`
- Supports display, simulation, theme, controls, and audio settings
- Persistent between sessions

### Design Patterns

**State Management**: App uses state enum to switch between MENU, SIMULATION, and SETTINGS modes

**Modular Plugin Architecture**: New algorithms are added by:
1. Creating folder under `src/simulations/[category]/[algorithm]/`
2. Implementing `*Visualizer.cpp` and `*Controller.cpp` classes
3. Registering with the menu system

**SFML Integration**: 60fps render loop with event handling, using SFML graphics, window, system, and audio modules

### Current Development Status

The project is in early development phases:
- **Phase 1 (Current)**: Basic project structure and SFML window setup
- **Phase 2 (Next)**: Quicksort visualization implementation
- **Phase 3**: User controls (play/pause/step/speed)
- **Phase 4**: Full theme integration
- **Phase 5**: Configuration system
- **Phase 6**: Expansion framework for new algorithms

### Key Technical Details

**C++ Standards**: Uses C++17 features, requires GCC 7+, Clang 5+, or MSVC 2017+

**Dependencies**: 
- SFML 2.5+ (graphics, window, system, audio)
- CMake 3.10+
- vcpkg for Windows dependency management

**Theme Specifications**:
- Primary: Bright green (#00FF41)
- Secondary: Amber (#FFA500) 
- Background: Dark green/black (#001100)
- Inactive: Dim green (#006600)
- Font: Courier New or similar monospace

**Performance Targets**: 60fps visualization with configurable array sizes (10-100 elements), animation delays (50ms-2000ms)

### File Organization

- `src/main.cpp`: Application entry point (currently basic SFML window)
- `include/core/Application.h`: Main application class definition
- `config/settings.json`: Runtime configuration
- `assets/fonts/`: Terminal-style fonts
- `learning/`: C++ educational materials and concepts
- `TODO_TRACKING.md`: Development progress tracking