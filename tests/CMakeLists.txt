# Prefer the system package (Arch: `catch2`); fall back to FetchContent for contributors
# without it installed locally. CI always has the real package, so the fallback only matters
# for local dev convenience, never for CI reproducibility.
find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
    )
    FetchContent_MakeAvailable(Catch2)
endif()

# One test binary for the project, compiling the same non-GUI source files the real
# executable uses (no Qt6::Widgets needed here — nothing under test touches a QWidget).
add_executable(archer_tests
    unit/test_extras.cpp
    unit/test_themes.cpp
    unit/test_resolution.cpp
    unit/test_updates.cpp
    unit/test_mirrorlist_parsing.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Extras.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Themes.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Resolution.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Updates.cpp
    ${CMAKE_SOURCE_DIR}/src/dialogs/MirrorListParsing.cpp
)

target_compile_definitions(archer_tests PRIVATE ENABLE_TEST_HOOKS)

target_include_directories(archer_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(archer_tests PRIVATE
    Qt6::Core
    Threads::Threads
    spdlog::spdlog
    fmt::fmt
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(archer_tests)
