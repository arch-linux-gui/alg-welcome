cmake_minimum_required(VERSION 3.16)

# Read version from the VERSION file (single source of truth)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" ARCHER_VERSION_STRING LIMIT_COUNT 1)

project(archer VERSION ${ARCHER_VERSION_STRING} LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Qt MOC, RCC, and UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Enable Link Time Optimization (IPO/LTO) when supported (Release builds)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
    message(STATUS "IPO/LTO is supported: enabling for Release builds")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
else()
    message(STATUS "IPO/LTO not supported: ${ipo_error}")
endif()

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Find threads package (for std::thread)
find_package(Threads REQUIRED)

# Find logging packages
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/WelcomeWindow.cpp
    src/dialogs/AboutUsDialog.cpp
    src/dialogs/MirrorListDialog.cpp
)

# Header files
set(HEADERS
    src/WelcomeWindow.h
    src/dialogs/AboutUsDialog.h
    src/dialogs/MirrorListDialog.h
)

# Utils source files
set(UTILS_SOURCES
    src/utils/Extras.cpp
    src/utils/Autostart.cpp
    src/utils/Themes.cpp
    src/utils/Resolution.cpp
    src/utils/Updates.cpp
    src/utils/Logging.cpp
)

# Utils header files
set(UTILS_HEADERS
    src/utils/Extras.h
    src/utils/Autostart.h
    src/utils/Themes.h
    src/utils/Resolution.h
    src/utils/Updates.h
    src/utils/Logging.h
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UTILS_SOURCES}
    ${UTILS_HEADERS}
)

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Threads::Threads
    spdlog::spdlog
    fmt::fmt
)

# Pass version from CMake to C++ as a compile definition
target_compile_definitions(${PROJECT_NAME} PRIVATE
    ARCHER_VERSION="${PROJECT_VERSION}"
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dialogs
)

# --- Tests ---
option(BUILD_TESTS "Build test utilities" ON)
if(BUILD_TESTS)
    add_executable(calamares_status_test
        tests/calamares_status.cpp
        src/utils/Extras.cpp
    )
    target_compile_definitions(calamares_status_test PRIVATE ENABLE_TEST_HOOKS)
    target_include_directories(calamares_status_test PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    target_link_libraries(calamares_status_test
        Qt6::Core
        Threads::Threads
        spdlog::spdlog
        fmt::fmt
    )
endif()

# Find threads package (for std::thread)
find_package(Threads REQUIRED)

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Install desktop file and assets
install(FILES archer.desktop
    DESTINATION share/applications
)

install(FILES assets/archer.png
    DESTINATION share/pixmaps
)

# Copy stylesheet to build directory
configure_file(styles.qss ${CMAKE_BINARY_DIR}/styles.qss COPYONLY)
install(FILES styles.qss
    DESTINATION share/archer
)
