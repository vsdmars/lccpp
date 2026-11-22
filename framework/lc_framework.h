#pragma once

#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <functional>
#include <ranges>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#ifdef _WIN32
#ifdef LC_CORE_EXPORTS
#define LC_API __declspec(dllexport)
#else
#define LC_API __declspec(dllimport)
#endif
#else
#define LC_API __attribute__((visibility("default")))
#endif

namespace lc {

// Formats common LeetCode types (scalars, strings, pointers, ranges/vectors)
// for readable assertion failure messages.
template <typename T>
std::string format_value(const T& val) {
  using D = std::remove_cvref_t<T>;
  std::ostringstream oss;
  if constexpr (std::same_as<D, bool>) {
    oss << (val ? "true" : "false");
  } else if constexpr (std::same_as<D, std::nullptr_t>) {
    oss << "nullptr";
  } else if constexpr (std::convertible_to<D, std::string_view>) {
    oss << '"' << std::string_view(val) << '"';
  } else if constexpr (std::ranges::input_range<D>) {
    oss << '[';
    bool first = true;
    for (const auto& elem : val) {
      if (!first) {
        oss << ", ";
      }
      first = false;
      oss << format_value(elem);
    }
    oss << ']';
  } else if constexpr (requires { oss << val; }) {
    oss << val;
  } else {
    oss << "<unprintable>";
  }
  return oss.str();
}

// Per-test-case result recorded by TestContext.
struct CaseResult {
  std::string name;
  bool passed = true;
  double elapsed_ms = 0.0;
  std::vector<std::string> failures;
};

// Lightweight test runner passed into every LeetCode problem's run() function.
class LC_API TestContext {
 public:
  explicit TestContext(std::string_view test_filter = "")
      : test_filter_(test_filter) {}

  // Executes one named test case, times it, and catches unexpected exceptions.
  template <std::invocable Fn>
  void test(std::string_view name, Fn&& fn) {
    if (!test_filter_.empty() && !name.contains(test_filter_)) {
      return;
    }

    CaseResult result;
    result.name = std::string(name);
    current_ = &result;

    const auto start = std::chrono::steady_clock::now();
    try {
      std::invoke(std::forward<Fn>(fn));
    } catch (const std::exception& e) {
      result.passed = false;
      result.failures.push_back(std::string("Uncaught exception: ") + e.what());
    } catch (...) {
      result.passed = false;
      result.failures.emplace_back("Uncaught unknown exception");
    }
    const auto end = std::chrono::steady_clock::now();
    result.elapsed_ms =
        std::chrono::duration<double, std::milli>(end - start).count();

    current_ = nullptr;
    results_.push_back(std::move(result));
  }

  // Asserts `actual == expected` and records file:line on mismatch.
  template <typename A, typename B>
    requires std::equality_comparable_with<A, B>
  void expect_eq(const A& actual, const B& expected,
                 std::source_location loc = std::source_location::current()) {
    if (!(actual == expected)) {
      fail(loc, "expected " + format_value(expected) + ", got " +
                    format_value(actual));
    }
  }

  // Asserts `actual != unexpected` and records file:line on failure.
  template <typename A, typename B>
    requires std::equality_comparable_with<A, B>
  void expect_ne(const A& actual, const B& unexpected,
                 std::source_location loc = std::source_location::current()) {
    if (actual == unexpected) {
      fail(loc, "expected value != " + format_value(unexpected));
    }
  }

  // Asserts `cond` is true and records file:line (and optional message) on failure.
  void expect_true(bool cond, std::string_view msg = "",
                   std::source_location loc = std::source_location::current()) {
    if (!cond) {
      fail(loc, msg.empty() ? "expected true, got false" : std::string(msg));
    }
  }

  // Asserts `cond` is false and records file:line on failure.
  void expect_false(bool cond, std::string_view msg = "",
                    std::source_location loc = std::source_location::current()) {
    if (cond) {
      fail(loc, msg.empty() ? "expected false, got true" : std::string(msg));
    }
  }

  [[nodiscard]] const std::vector<CaseResult>& results() const noexcept {
    return results_;
  }

  [[nodiscard]] bool all_passed() const noexcept;
  [[nodiscard]] std::size_t passed_count() const noexcept;

 private:
  void fail(std::source_location loc, std::string detail);

  std::string test_filter_;
  CaseResult* current_ = nullptr;
  std::vector<CaseResult> results_;
};

// C++23 concept for a self-describing LeetCode problem type.
template <typename P>
concept LeetCodeProblem = requires(TestContext& ctx) {
  { P::kId } -> std::convertible_to<int>;
  { P::kTitle } -> std::convertible_to<std::string_view>;
  { P::kCategory } -> std::convertible_to<std::string_view>;
  { P::run(ctx) } -> std::same_as<void>;
};

// Runtime descriptor stored in the global Registry for each loaded problem.
struct ProblemEntry {
  int id = 0;
  std::string title;
  std::string category;
  void (*run)(TestContext&) = nullptr;
};

// Shared singleton registry (exported from liblc_core) populated automatically
// when liblc_problems is dynamically loaded via dlopen.
class LC_API Registry {
 public:
  static Registry& instance();

  void add(ProblemEntry entry);

  // Returns all registered problems sorted ascending by LeetCode ID.
  [[nodiscard]] std::vector<const ProblemEntry*> sorted_all() const;
  [[nodiscard]] const ProblemEntry* find_by_id(int id) const;

 private:
  Registry() = default;
  std::vector<ProblemEntry> entries_;
};

// Static self-registration helper constrained by the LeetCodeProblem concept.
template <LeetCodeProblem P>
struct Registrar {
  Registrar() noexcept {
    try {
      Registry::instance().add(ProblemEntry{
          .id = P::kId,
          .title = std::string(P::kTitle),
          .category = std::string(P::kCategory),
          .run = &P::run,
      });
    } catch (...) {
      std::fputs("lc::Registrar failed during static initialization\n", stderr);
    }
  }
};

}  // namespace lc

// Convenience macro for defining and auto-registering a LeetCode problem inside
// an anonymous namespace in its `.cpp` file.
//
// Example:
//   LC_PROBLEM(133, "Clone Graph", "graph", ctx) {
//     ctx.test("basic", [&] { ... });
//   }
#define LC_PROBLEM(ID, TITLE, CATEGORY, CTX_VAR)                        \
  struct LcProblem_##ID {                                               \
    static constexpr int kId = (ID);                                    \
    static constexpr std::string_view kTitle = (TITLE);                 \
    static constexpr std::string_view kCategory = (CATEGORY);           \
    static void run(::lc::TestContext&);                                \
  };                                                                    \
  static const ::lc::Registrar<LcProblem_##ID> kLcRegistrar_##ID{};     \
  /* NOLINTNEXTLINE(bugprone-macro-parentheses) */                      \
  void LcProblem_##ID::run(::lc::TestContext& CTX_VAR)
