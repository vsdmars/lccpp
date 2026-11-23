// LeetCode 686: Repeated String Match
// https://leetcode.com/problems/repeated-string-match/
//
// Problem:
//   Given two strings `a` and `b`, return the minimum number of times `a` must
//   be repeated (concatenated with itself) so that `b` appears as a contiguous
//   substring of the repeated string. If no number of repetitions can contain
//   `b`, return -1.
//
// Algorithm:
//   To contain `b`, the repeated string must be at least as long as `b`, so the
//   minimum candidate repetition count is `min_reps = ceil(|b| / |a|)`.
//   Because `b` can start at any offset r in [0, |a| - 1] inside the first copy
//   of `a`, the match spans indices [r, r + |b| - 1] and therefore needs at
//   most `ceil((r + |b|) / |a|) <= ceil((|a| - 1 + |b|) / |a|) <= min_reps + 1`
//   copies of `a`. Any further repetition only duplicates periodic suffixes
//   already present inside `min_reps + 1` copies.
//   Thus we build `a` repeated `min_reps` times (reserving space for
//   `min_reps + 1` copies up front), check if it contains `b`, and if not,
//   append one final copy of `a` and check once more.
//   Time Complexity:  O((|a| + |b|) * |b|) worst-case using std::string::contains
//                     (linear O(|a| + |b|) on typical inputs; KMP would bound
//                     worst-case to O(|a| + |b|)).
//   Space Complexity: O(|a| + |b|) to hold at most `min_reps + 1` copies of `a`.
//
// Data Structures:
//   - std::string: pre-reserved buffer holding the repeated copies of `a` so
//     concatenation incurs no intermediate reallocations.
//
// Real-World Applications:
//   - Detecting cyclic / periodic pattern matches across ring buffers and
//     wrapped network packet streams.
//   - DNA tandem-repeat alignment and circular genome substring matching.
//   - Verifying tiled texture or repeating-key cipherstreams against a target
//     byte signature.
//
// Related LeetCode Problems:
//   - #28 Find the Index of the First Occurrence in a String [done] (core substring search)
//   - #459 Repeated Substring Pattern [new] (checking if a string is a periodic repeat)
//   - #796 Rotate String [new] (substring check inside a doubled string `s + s`)
//   - #1668 Maximum Repeating Substring [new] (longest consecutive repetition of a pattern)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns the minimum repetition count of `a` containing `b` as a substring,
  // or -1 if impossible.
  // Time: O((|a| + |b|) * |b|) worst-case, Space: O(|a| + |b|).
  int repeatedStringMatch(std::string_view a, std::string_view b) {
    if (b.empty()) {
      return 0;
    }
    if (a.empty()) {
      return -1;
    }

    // Minimum repetitions needed so that repeated.size() >= b.size():
    // ceil(|b| / |a|) = (|b| + |a| - 1) / |a|.
    const std::size_t min_reps = ((b.size() + a.size()) - 1uz) / a.size();

    // Reserve enough capacity for `min_reps + 1` copies to avoid reallocating
    // if the first check misses and we append one extra copy.
    std::string repeated;
    repeated.reserve((min_reps + 1uz) * a.size());
    for (std::size_t i = 0uz; i < min_reps; ++i) {
      repeated.append(a);
    }

    // Check `min_reps` copies first.
    if (repeated.contains(b)) {
      return static_cast<int>(min_reps);
    }

    // A valid match starting partway into the first copy of `a` can spill into
    // at most one additional copy of `a`.
    repeated.append(a);
    if (repeated.contains(b)) {
      return static_cast<int>(min_reps + 1uz);
    }

    return -1;
  }
};

LC_PROBLEM(686, "Repeated String Match", "string", ctx) {
  Solution sol;

  ctx.test("example_1_three_repeats", [&] {
    ctx.expect_eq(sol.repeatedStringMatch("abcd", "cdabcdab"), 3);
  });

  ctx.test("overlapping_prefix_suffix_from_source", [&] {
    ctx.expect_eq(sol.repeatedStringMatch("abababaaba", "aabaaba"), 2);
  });

  ctx.test("single_char_exact_multiple", [&] {
    ctx.expect_eq(sol.repeatedStringMatch("a", "aa"), 2);
  });

  ctx.test("identical_strings", [&] {
    ctx.expect_eq(sol.repeatedStringMatch("a", "a"), 1);
  });

  ctx.test("b_already_inside_single_a", [&] {
    ctx.expect_eq(sol.repeatedStringMatch("abcdef", "cde"), 1);
  });

  ctx.test("requires_min_reps_plus_one", [&] {
    ctx.expect_eq(sol.repeatedStringMatch("abc", "cabcabca"), 4);
  });

  ctx.test("impossible_match", [&] {
    ctx.expect_eq(sol.repeatedStringMatch("abc", "wxyz"), -1);
  });
}

}  // namespace
