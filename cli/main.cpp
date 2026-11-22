#include "framework/lc_framework.h"

#include <CLI/CLI.hpp>

#include <algorithm>
#include <cctype>
#include <dlfcn.h>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

// Minimal RAII wrapper around POSIX dlopen/dlclose for dynamically loading the
// compiled problems shared library (liblc_problems.dylib / .so).
class DynamicLib {
 public:
  explicit DynamicLib(const std::string& path)
      : handle_(dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL)) {
    if (handle_ == nullptr) {
      const char* err = dlerror();
      throw std::runtime_error("Failed to load problems library '" + path +
                               "': " + (err != nullptr ? err : "unknown error"));
    }
  }

  ~DynamicLib() {
    if (handle_ != nullptr) {
      dlclose(handle_);
    }
  }

  DynamicLib(const DynamicLib&) = delete;
  DynamicLib& operator=(const DynamicLib&) = delete;

 private:
  void* handle_ = nullptr;
};

bool icontains(std::string_view haystack, std::string_view needle) {
  const auto it = std::ranges::search(
      haystack, needle, [](unsigned char a, unsigned char b) {
        return std::tolower(a) == std::tolower(b);
      });
  return !it.empty();
}

bool run_problem(const lc::ProblemEntry& problem, std::string_view test_filter) {
  std::cout << "==> #" << problem.id << " " << problem.title << " ["
            << problem.category << "]\n";

  lc::TestContext ctx(test_filter);
  problem.run(ctx);

  if (ctx.results().empty()) {
    std::cout << "    (no test cases matched)\n";
    return true;
  }

  for (const auto& r : ctx.results()) {
    std::cout << "    [" << (r.passed ? "PASS" : "FAIL") << "] " << r.name
              << " (" << std::fixed << std::setprecision(3) << r.elapsed_ms
              << " ms)\n";
    for (const auto& msg : r.failures) {
      std::cout << "           " << msg << '\n';
    }
  }

  std::cout << "    Summary: " << ctx.passed_count() << "/"
            << ctx.results().size() << " passed\n";
  return ctx.all_passed();
}

struct CliOptions {
  bool list_only = false;
  bool run_all = false;
  std::vector<int> problem_ids;
  std::string name_filter;
  std::string category_filter;
  std::string test_filter;
  std::string lib_path = LC_DEFAULT_PROBLEMS_LIB;
};

std::vector<const lc::ProblemEntry*> select_problems(
    const lc::Registry& registry,
    const std::vector<const lc::ProblemEntry*>& all_problems,
    const CliOptions& opts) {
  if (opts.run_all) {
    return all_problems;
  }

  std::vector<const lc::ProblemEntry*> selected;
  if (!opts.problem_ids.empty()) {
    for (int id : opts.problem_ids) {
      const auto* p = registry.find_by_id(id);
      if (p == nullptr) {
        throw std::runtime_error("LeetCode problem #" + std::to_string(id) +
                                 " not found in " + opts.lib_path);
      }
      selected.push_back(p);
    }
    return selected;
  }

  for (const auto* p : all_problems) {
    const bool cat_ok =
        opts.category_filter.empty() || p->category == opts.category_filter;
    const bool name_ok =
        opts.name_filter.empty() || icontains(p->title, opts.name_filter);
    if (cat_ok && name_ok) {
      selected.push_back(p);
    }
  }
  return selected;
}

}  // namespace

int main(int argc, char** argv) try {
  CLI::App app{"lc - C++23 LeetCode problem loader & test runner"};
  CliOptions opts;

  app.add_flag("-l,--list", opts.list_only, "List all loaded LeetCode problems");
  app.add_option("-p,--problem", opts.problem_ids,
                 "Run specific problem(s) by LeetCode number (e.g. -p 133)");
  app.add_option("-n,--name", opts.name_filter,
                 "Run problems whose title contains this substring");
  app.add_option("-c,--category", opts.category_filter,
                 "Run all problems in a category (e.g. graph, array)");
  app.add_flag("-a,--all", opts.run_all, "Run all registered problems");
  app.add_option("-t,--test", opts.test_filter,
                 "Filter individual test cases by substring");
  app.add_option("--lib", opts.lib_path, "Path to the problems shared library")
      ->capture_default_str();

  CLI11_PARSE(app, argc, argv);

  const bool has_selector = opts.run_all || !opts.problem_ids.empty() ||
                            !opts.name_filter.empty() ||
                            !opts.category_filter.empty();
  if (!opts.list_only && !has_selector) {
    std::cout << app.help();
    return 0;
  }

  // Dynamically load liblc_problems; static registrars inside each problem .o
  // automatically register into lc::Registry::instance().
  DynamicLib problems_lib(opts.lib_path);

  const auto& registry = lc::Registry::instance();
  const auto all_problems = registry.sorted_all();

  if (opts.list_only) {
    for (const auto* p : all_problems) {
      std::cout << std::setw(4) << p->id << "  [" << p->category << "] "
                << p->title << '\n';
    }
    std::cout << "Total: " << all_problems.size() << " problem(s)\n";
    return 0;
  }

  const auto selected = select_problems(registry, all_problems, opts);
  if (selected.empty()) {
    std::cerr << "No matching problems found. Use --list to see available "
                 "problems.\n";
    return 1;
  }

  bool ok = true;
  for (const auto* p : selected) {
    ok = run_problem(*p, opts.test_filter) && ok;
  }
  return ok ? 0 : 1;
} catch (const std::exception& e) {
  std::cerr << "Error: " << e.what() << '\n';
  return 1;
}
