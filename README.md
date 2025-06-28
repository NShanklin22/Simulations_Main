# C++ Algorithm Simulation App

A modular C++ application featuring interactive algorithm visualizations with a Fallout terminal theme.

## Quick Start

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10 or higher
- SFML 2.5+ (Simple and Fast Multimedia Library)
- vcpkg (recommended for Windows dependency management)

### SFML Installation (Windows with vcpkg)
```bash
# Install vcpkg if not already installed
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install SFML
.\vcpkg install sfml:x64-windows
```

### Building
```bash
# Configure with vcpkg toolchain
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]\scripts\buildsystems\vcpkg.cmake

# Build
cmake --build . --config Release
```

### Running
```bash
./SimulationApp  # or SimulationApp.exe on Windows
```

## Project Structure
```
├── src/                   # Source code
├── include/              # Header files  
├── assets/               # Fonts and audio resources
├── config/               # Configuration files
├── docs/                 # Documentation and learning materials
└── CMakeLists.txt        # Build configuration
```

## Features
- **Interactive Visualizations**: Real-time and step-by-step algorithm execution
- **Fallout Terminal Theme**: Retro green/amber terminal aesthetic
- **User Controls**: Play, pause, step-through, and speed adjustment
- **Modular Design**: Easy to add new algorithms
- **Persistent Settings**: Configuration saves between sessions

## Current Algorithms
- Quicksort (Bar chart visualization)

## Documentation
- `PROJECT_OVERVIEW.md` - Detailed project specifications
- `TODO_TRACKING.md` - Development progress tracking
- `docs/cpp_explanations.md` - C++ concepts and learning materials

## Contributing
This project is designed with modularity in mind. New algorithms can be added by:
1. Creating a new folder under `src/simulations/[category]/`
2. Implementing the visualizer and controller classes
3. Registering the algorithm in the main menu system

## License
[Add your preferred license here]