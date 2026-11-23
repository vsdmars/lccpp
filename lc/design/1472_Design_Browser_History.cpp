// LeetCode 1472: Design Browser History
// https://leetcode.com/problems/design-browser-history/
//
// Problem:
//   Design a single-tab `BrowserHistory` class initialized at `homepage`:
//     - `visit(url)`: navigate from the current page to `url`, discarding all
//       forward history beyond the current page.
//     - `back(steps)`: move at most `steps` pages backward in history and
//       return the resulting current URL.
//     - `forward(steps)`: move at most `steps` pages forward in history and
//       return the resulting current URL.
//
// Algorithm:
//   Store visited URLs in a contiguous `std::vector<std::string> history_`
//   alongside two indices: `current_` (the active page) and `last_valid_` (the
//   furthest reachable forward page).
//     - `visit(url)`: advance `++current_`. If `current_ < history_.size()`,
//       overwrite `history_[current_]` in place (reusing allocated capacity);
//       otherwise `push_back(url)`. Set `last_valid_ = current_` to logically
//       clear all forward history in O(1) time without erasing trailing slots.
//     - `back(steps)`: clamp `current_` downward to `0` safely without unsigned
//       underflow, then return `history_[current_]`.
//     - `forward(steps)`: clamp `current_` upward to `last_valid_`, then return
//       `history_[current_]`.
//   Time Complexity:  O(1) amortized for `visit`, and O(1) worst-case for both
//                     `back` and `forward` (unlike a linked list where `back`
//                     and `forward` take O(steps)).
//   Space Complexity: O(V * L) where V is the peak history depth and L is URL
//                     length.
//
// Data Structures:
//   - `std::vector<std::string> history_`: random-access dynamic array storing
//     chronological session URLs.
//   - `std::size_t current_` and `last_valid_`: cursor and logical high-water
//     mark supporting O(1) forward truncation and O(1) multi-step jumps.
//
// Real-World Applications:
//   - Web browser tab session history (Chromium/Firefox navigation stacks).
//   - Undo/redo command histories in IDEs, CAD tools, and document editors
//     where a new edit after undo discards the redo branch.
//   - File-manager and mobile navigation stacks supporting multi-step jumps.
//
// Related LeetCode Problems:
//   - #146 LRU Cache [done] (stateful cache design with O(1) operations)
//   - #2296 Design a Text Editor [new] (cursor movement and history editing)
//   - #707 Design Linked List [new] (alternative doubly-linked navigation model)
//   - #1396 Design Underground System [done] (stateful O(1) system design class)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace {

// Single-tab browser navigation history supporting O(1) amortized `visit` and
// O(1) arbitrary-step `back` and `forward` jumps.
class BrowserHistory {
 public:
  // Initializes the browser tab at `homepage`.
  explicit BrowserHistory(std::string homepage) {
    history_.push_back(std::move(homepage));
  }

  // Navigates to `url` from the current page and clears all forward history.
  // Time: O(1) amortized, Space: O(1) amortized.
  void visit(std::string url) {
    ++current_;
    if (current_ < history_.size()) {
      // Reuse an existing slot from discarded forward history in O(1).
      history_[current_] = std::move(url);
    } else {
      history_.push_back(std::move(url));
    }
    // Truncate forward history logically so old slots beyond current_ are
    // unreachable.
    last_valid_ = current_;
  }

  // Moves up to `steps` pages backward and returns the resulting URL.
  // Time: O(1), Space: O(1).
  std::string back(int steps) {
    const auto step_count = static_cast<std::size_t>(steps);
    current_ = (step_count >= current_) ? 0uz : (current_ - step_count);
    return history_[current_];
  }

  // Moves up to `steps` pages forward and returns the resulting URL.
  // Time: O(1), Space: O(1).
  std::string forward(int steps) {
    const auto step_count = static_cast<std::size_t>(steps);
    current_ = std::min(last_valid_, current_ + step_count);
    return history_[current_];
  }

 private:
  std::vector<std::string> history_;
  std::size_t current_ = 0uz;
  std::size_t last_valid_ = 0uz;
};

LC_PROBLEM(1472, "Design Browser History", "design", ctx) {
  ctx.test("example_1", [&] {
    BrowserHistory bh("leetcode.com");
    bh.visit("google.com");
    bh.visit("facebook.com");
    bh.visit("youtube.com");

    ctx.expect_eq(bh.back(1), "facebook.com");
    ctx.expect_eq(bh.back(1), "google.com");
    ctx.expect_eq(bh.forward(1), "facebook.com");

    bh.visit("linkedin.com");
    ctx.expect_eq(bh.forward(2), "linkedin.com");
    ctx.expect_eq(bh.back(2), "google.com");
    ctx.expect_eq(bh.back(7), "leetcode.com");
  });

  ctx.test("bounds_on_homepage_only", [&] {
    BrowserHistory bh("home.dev");
    ctx.expect_eq(bh.back(1), "home.dev");
    ctx.expect_eq(bh.back(100), "home.dev");
    ctx.expect_eq(bh.forward(1), "home.dev");
    ctx.expect_eq(bh.forward(100), "home.dev");
  });

  ctx.test("repeated_truncation_and_full_rewind", [&] {
    BrowserHistory bh("a.com");
    bh.visit("b.com");
    bh.visit("c.com");
    bh.visit("d.com");

    // Rewind all the way to homepage, then branch into a new single-page chain.
    ctx.expect_eq(bh.back(10), "a.com");
    bh.visit("x.com");
    // Old forward entries ("c.com", "d.com") must remain unreachable.
    ctx.expect_eq(bh.forward(5), "x.com");
    ctx.expect_eq(bh.back(1), "a.com");
    ctx.expect_eq(bh.forward(1), "x.com");

    // Grow past the original vector capacity after overwriting.
    bh.visit("y.com");
    bh.visit("z.com");
    bh.visit("w.com");
    ctx.expect_eq(bh.back(2), "y.com");
    ctx.expect_eq(bh.forward(10), "w.com");
  });
}

}  // namespace
