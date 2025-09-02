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

This is a C++17 SFML-based algorithm visualization application optimized for YouTube content creation, featuring accurate educational algorithms with retro ASMR audio and satisfying visual effects.

### Core Architecture Components

**Application Layer (`src/core/`)**:
- `Application` class manages the main game loop, window, and core systems
- `MenuSystem` handles navigation with audio feedback and terminal styling
- `AudioManager` provides procedural retro ASMR sound generation
- `ConfigManager` handles JSON-based persistent settings for display, audio, and controls

**Simulation Layer (`src/simulations/`)**:
- Organized by algorithm category (`sorting/quicksort/` implemented)
- Each algorithm has a `Visualizer` (rendering) and `Controller` (logic) class
- **QuickSort**: Complete Hoare partitioning implementation with two-pointer visualization
- **Documentation**: Each algorithm includes detailed color logic and educational notes

**Audio System (`AudioManager`)**:
- Procedural sound generation using sine waves and triangle wave synthesis
- Retro video game aesthetic with ASMR quality (warm, non-harsh)
- Value-to-pitch mapping for musical algorithm representation
- Menu navigation sounds, algorithm step sounds, and completion fanfares

**Configuration System**:
- JSON-based settings in `config/settings.json` and `config/theme.json`
- Supports display (1200x800 default), simulation speeds, theme colors, controls, and audio
- Audio enable/disable, master volume, and sound effect volume controls

### Design Patterns

**State Management**: App uses state enum to switch between MENU, SIMULATION, and SETTINGS modes

**Modular Plugin Architecture**: New algorithms are added by:
1. Creating folder under `src/simulations/[category]/[algorithm]/`
2. Implementing `*Visualizer.cpp` and `*Controller.cpp` classes
3. Registering with the menu system

**Current Implementation Status**: 
- **QuickSort**: Complete Hoare partitioning with proper two-pointer algorithm visualization
- **Menu System**: Full navigation with audio feedback and terminal styling
- **Audio Integration**: Retro ASMR sound system with procedural generation
- **Speed Controls**: Flexible timing from 50ms (fast demo) to 5000ms (detailed study)
- **Visual Design**: Clean, focused highlighting without excessive visual clutter

**SFML Integration**: 60fps render loop with SFML 3.x audio, graphics, window, and system modules

### Current Development Status

**Phase 5 Completed** - Ready for cinematic visual enhancement:
- **Phase 1 (✅)**: SFML 3.x integration and basic window setup
- **Phase 2 (✅)**: Quicksort visualization implementation with basic controls
- **Phase 3 (✅)**: Menu system and simulation selection with configuration integration
- **Phase 4 (✅)**: Speed controls and enhanced user interaction
- **Phase 5 (✅)**: Audio integration, algorithm accuracy overhaul, and visual highlighting improvements
- **Phase 6 (Current)**: Cinematic visual enhancement for YouTube content creation
- **Future Phases**: Algorithm library expansion, export pipeline for Adobe integration

### Key Technical Details

**C++ Standards**: Uses C++17 features, requires GCC 7+, Clang 5+, or MSVC 2017+

**Dependencies**: 
- SFML 3.x (graphics, window, system, audio) via vcpkg
- CMake 3.10+
- vcpkg for Windows dependency management
- Windows SDK 10.0.22621.0+ for platform APIs

**Theme Specifications (Fallout Terminal)**:
- **Brightest Green** (#00FF41): Active operations, current algorithm focus
- **Medium Green** (#64FF64): Pivot elements, important context
- **Subtle Green** (#008C1E): Background context when needed
- **Dim Green** (#006600): Inactive elements
- **Background**: Dark green/black (#001100)
- **Font**: System fallbacks (Consola, Arial, Calibri) with terminal.ttf preference

**Performance Targets**: 
- 60fps visualization with configurable array sizes (10-100 elements)
- Animation delays: 50ms-5000ms (0.05s to 5s per step)
- Audio: 44.1kHz procedural generation with real-time synthesis
- Memory: Efficient bar rendering with RAII resource management

### File Organization

**Core Application**:
- `src/main.cpp`: Application entry point
- `include/core/Application.h` & `src/core/Application.cpp`: Main application class with state management
- `include/core/MenuSystem.h` & `src/core/MenuSystem.cpp`: Terminal-style menu with audio feedback
- `include/core/AudioManager.h` & `src/core/AudioManager.cpp`: Procedural retro ASMR sound generation
- `include/core/ConfigManager.h` & `src/core/ConfigManager.cpp`: JSON configuration management

**QuickSort Implementation**:
- `include/simulations/sorting/quicksort/QuicksortController.h` & `.cpp`: Hoare partitioning algorithm
- `include/simulations/sorting/quicksort/QuicksortVisualizer.h` & `.cpp`: Two-pointer visualization
- `src/simulations/sorting/quicksort/QuicksortColorLogic.md`: Educational documentation

**Configuration & Assets**:
- `config/settings.json`: Runtime configuration (display, audio, controls)
- `config/theme.json`: Theme color specifications
- `assets/fonts/`: Terminal-style fonts (with system fallbacks)
- `assets/sounds/`: Audio assets directory (currently using procedural generation)

**Documentation**:
- `TODO_TRACKING.md`: Phase-by-phase development progress tracking
- `CLAUDE.md`: This file - development guidance and architecture overview
- `learning/`: C++ educational materials and algorithm concepts

### YouTube Content Creation Focus

**Current Status**: Ready for Phase 6 - Cinematic Visual Enhancement
- **Target**: High-quality algorithm visualization content for educational YouTube videos
- **Audio**: Retro ASMR sounds suitable for repeated viewing
- **Visual**: Clean, focused highlighting perfect for screen recording with OBS
- **Algorithm**: Accurate QuickSort implementation matching educational standards
- **Speed Control**: Flexible timing for both detailed explanation and quick demonstration