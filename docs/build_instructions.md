# Build Instructions

## Windows Development Setup

### Prerequisites
1. **Visual Studio 2019/2022** with C++ development tools
2. **Git** for version control
3. **CMake** 3.10 or higher

### SFML Setup with vcpkg

#### 1. Install vcpkg
```bash
# Clone vcpkg to a permanent location (e.g., C:\vcpkg)
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg

# Bootstrap vcpkg
.\bootstrap-vcpkg.bat

# Integrate with Visual Studio (optional but recommended)
.\vcpkg integrate install
```

#### 2. Install SFML
```bash
# For 64-bit Windows
.\vcpkg install sfml:x64-windows

# For 32-bit Windows (if needed)
.\vcpkg install sfml:x86-windows
```

### Building the Project

#### Command Line Build
```bash
# Navigate to project directory
cd /path/to/Simulations_Main

# Create build directory
mkdir build
cd build

# Configure with vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release
```

#### Visual Studio Build
1. Open Visual Studio
2. File → Open → CMake → Select CMakeLists.txt
3. Configure CMake settings to use vcpkg toolchain
4. Build → Build All

### Running the Application
```bash
# From build directory
.\Release\SimulationApp.exe

# Or double-click the executable in build/Release/
```

### Troubleshooting

#### SFML Not Found
- Ensure vcpkg is properly installed and integrated
- Verify SFML installation: `.\vcpkg list | findstr sfml`
- Check CMake toolchain file path

#### Build Errors
- Ensure C++17 support is enabled
- Check that all required SFML modules are installed
- Verify Windows SDK version compatibility

#### Runtime Errors
- Ensure SFML DLLs are in PATH or beside executable
- Check that assets and config directories are copied to build folder

### Development Workflow
1. Make code changes
2. Run `cmake --build . --config Release` from build directory
3. Test executable
4. Repeat

### Performance Build Options
- Release: `-DCMAKE_BUILD_TYPE=Release` (optimized)
- Debug: `-DCMAKE_BUILD_TYPE=Debug` (with debug symbols)
- RelWithDebInfo: `-DCMAKE_BUILD_TYPE=RelWithDebInfo` (optimized + debug symbols)