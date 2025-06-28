# C++ Concepts for Algorithm Simulation Project

## Essential C++ Concepts We'll Use

### 1. Classes and Objects
**What**: Blueprint for creating objects with data and functions
**Why We Need It**: Every component (Menu, Visualizer, Theme) will be a class
**Example Use**: `QuickSortVisualizer` class to handle sorting display

### 2. Headers and Source Files
**What**: Separate declarations (.h) from implementations (.cpp)
**Why We Need It**: Organize code, faster compilation, clean interfaces
**Project Structure**: 
- `include/` folder for .h files
- `src/` folder for .cpp files

### 3. Namespaces
**What**: Organize code into logical groups
**Why We Need It**: Prevent naming conflicts, organize features
**Example**: `namespace Simulations::Sorting`

### 4. STL Containers
**What**: Pre-built data structures (vector, map, etc.)
**Why We Need It**: Store arrays to sort, configuration settings
**Key Ones We'll Use**:
- `std::vector<int>` - Dynamic arrays for sorting
- `std::map<string, string>` - Configuration key-value pairs

### 5. Memory Management
**What**: How C++ handles memory allocation
**Why We Need It**: SFML objects, large data arrays
**Modern Approach**: Smart pointers (`std::unique_ptr`, `std::shared_ptr`)

### 6. File I/O
**What**: Reading and writing files
**Why We Need It**: Configuration files, settings persistence
**Libraries**: `<fstream>`, JSON parsing

### 7. Templates (Basic)
**What**: Write code that works with different types
**Why We Need It**: Generic algorithms, flexible containers
**Example**: Template function for different sort types

## Project-Specific Patterns

### SFML Integration
```cpp
// Basic SFML window pattern we'll use
sf::RenderWindow window(sf::VideoMode(800, 600), "Simulations");
while (window.isOpen()) {
    // Handle events
    // Update logic  
    // Draw everything
    window.display();
}
```

### State Management
```cpp
// How we'll handle different screens
enum class AppState { MENU, SORTING, SETTINGS };
AppState currentState = AppState::MENU;
```

### Configuration Pattern
```cpp
// How we'll handle settings
class ConfigManager {
    std::map<std::string, std::string> settings;
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename);
};
```

## Learning Progression

### Phase 1: Syntax Basics
- Variables and data types
- Functions and parameters
- Basic classes and constructors

### Phase 2: Project Patterns
- SFML window creation
- Event handling loops
- Drawing shapes and text

### Phase 3: Data Management
- STL containers for arrays
- File reading/writing
- JSON configuration

### Phase 4: Architecture
- Class hierarchies
- Modular design
- Clean interfaces

## Common C++ Gotchas to Watch For

1. **Memory Leaks**: Use smart pointers, not raw `new`/`delete`
2. **Header Guards**: Prevent multiple includes
3. **Const Correctness**: Mark things that don't change as `const`
4. **Initialization**: Always initialize variables
5. **RAII**: Resource management through constructors/destructors

## Next Steps
1. Set up a simple "Hello World" with SFML
2. Create a basic window that responds to close events
3. Draw simple shapes (preparation for bar charts)
4. Practice with vectors and basic algorithms

Each concept will be introduced with small examples before using in the main project!