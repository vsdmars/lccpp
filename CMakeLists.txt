# Require CMake 3.25+ for C++23 (`cxx_std_23`) support.
cmake_minimum_required(VERSION 3.25)

project(
  lccpp
  VERSION 0.1.0
  DESCRIPTION "C++23 LeetCode problem loader and test runner"
  LANGUAGES CXX
)

# Emit `build/compile_commands.json` so clangd / Neovim gets full C++23 flags.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optional build-time clang-tidy check (per-target so CLI11 isn't linted).
# Usage: cmake -S . -B build -G Ninja -DLC_ENABLE_CLANG_TIDY=ON
option(LC_ENABLE_CLANG_TIDY "Run clang-tidy during build" OFF)
if(LC_ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
  set(LC_CLANG_TIDY_CMD
    ${CLANG_TIDY_EXE}
    "--config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy"
  )
  if(APPLE AND CMAKE_OSX_SYSROOT)
    list(APPEND LC_CLANG_TIDY_CMD
      "--extra-arg=-isysroot"
      "--extra-arg=${CMAKE_OSX_SYSROOT}"
    )
  endif()
endif()

# ------------------------------------------------------------------------------
# Fetch CLI11 (header-only modern C++ CLI parser)
# ------------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG v2.7.2
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(cli11)

# ------------------------------------------------------------------------------
# 1. lc_core (SHARED): holds the single Registry singleton and TestContext.
#    Making it SHARED guarantees both the `lc` CLI and the dynamically loaded
#    `liblc_problems` library share the exact same Registry::instance().
# ------------------------------------------------------------------------------
add_library(lc_core SHARED framework/lc_framework.cpp)
target_compile_features(lc_core PUBLIC cxx_std_23)
target_include_directories(lc_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(lc_core PRIVATE LC_CORE_EXPORTS)
target_compile_options(lc_core PRIVATE -Wall -Wextra -Wpedantic)

# ------------------------------------------------------------------------------
# 2. lc_problems (SHARED): compiles every LeetCode problem `.cpp` under `lc/`
#    into its own `.o` file and bundles them into one shared library loaded via
#    `dlopen()`.
#    - Editing one problem only recompiles that single `.o` + relinks this lib
#      (the `lc` binary itself does not need to relink).
#    - Using SHARED (instead of STATIC .a) prevents the linker from dropping
#      unreferenced static self-registration objects.
#    - CONFIGURE_DEPENDS re-scans automatically when you add a new `.cpp` file.
# ------------------------------------------------------------------------------
file(
  GLOB_RECURSE LC_PROBLEM_SOURCES
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/lc/*.cpp"
)

add_library(lc_problems SHARED ${LC_PROBLEM_SOURCES})
target_link_libraries(lc_problems PRIVATE lc_core)
target_compile_options(lc_problems PRIVATE -Wall -Wextra -Wpedantic)

# ------------------------------------------------------------------------------
# 3. lc (CLI executable): uses CLI11 to parse flags and `dlopen()` to load
#    `liblc_problems` at runtime.
# ------------------------------------------------------------------------------
add_executable(lc cli/main.cpp)
target_link_libraries(lc PRIVATE lc_core CLI11::CLI11 ${CMAKE_DL_LIBS})
target_compile_definitions(
  lc
  PRIVATE LC_DEFAULT_PROBLEMS_LIB="$<TARGET_FILE:lc_problems>"
)
target_compile_options(lc PRIVATE -Wall -Wextra -Wpedantic)

# Ensure `cmake --build build` always builds `lc_problems` alongside `lc`.
add_dependencies(lc lc_problems)

if(LC_ENABLE_CLANG_TIDY)
  set_target_properties(
    lc_core lc_problems lc
    PROPERTIES CXX_CLANG_TIDY "${LC_CLANG_TIDY_CMD}"
  )
endif()
