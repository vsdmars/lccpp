# lccpp

Simple C++23 LeetCode problem runner powered by [CLI11](https://github.com/CLIUtils/CLI11) and dynamic library loading (`dlopen`).

## Layout

```text
lccpp/
├── CMakeLists.txt
├── framework/
│   ├── lc_framework.h      # TestContext, LeetCodeProblem concept, Registry, LC_PROBLEM macro
│   └── lc_framework.cpp    # Shared Registry singleton (liblc_core)
├── cli/
│   └── main.cpp            # CLI11 frontend + dlopen loader for liblc_problems
└── lc/
    ├── array/
    │   └── 1_Two_Sum.cpp
    └── graph/
        └── 133_Clone_Graph.cpp
```

## Build & Run

```bash
cmake -S . -B build -G Ninja
cmake --build build

# Optional: run Google-style clang-tidy during build
cmake -S . -B build -G Ninja -DLC_ENABLE_CLANG_TIDY=ON
cmake --build build --clean-first

./build/lc --list              # list all problems sorted by LeetCode ID
./build/lc -p 133              # run #133 Clone Graph
./build/lc -p 1 133            # run multiple problems
./build/lc -c graph            # run all problems in graph/
./build/lc -n clone            # run problems matching title substring
./build/lc -p 133 -t review    # filter individual test cases
./build/lc --all               # run all problems
```

## Adding a New Problem

Drop a `.cpp` file into any category folder under `lc/` (e.g., `lc/graph/207_Course_Schedule.cpp`) and run `cmake --build build`:

```cpp
#include "framework/lc_framework.h"

namespace {

class Solution {
 public:
  bool canFinish(int numCourses, ...) { ... }
};

LC_PROBLEM(207, "Course Schedule", "graph", ctx) {
  Solution sol;
  ctx.test("example 1", [&] {
    ctx.expect_true(sol.canFinish(...));
  });
}

}  // namespace
```

Wrap `Solution` / helper structs in an anonymous `namespace { ... }` so multiple problems can freely define `class Solution` or `struct Node` without symbol collisions.
