// LeetCode 1209: Remove All Adjacent Duplicates In String II
// https://leetcode.com/problems/remove-all-adjacent-duplicates-in-string-ii/
//
// Problem:
//   Given a lowercase English string `s` and an integer `k` (k >= 2),
//   repeatedly delete any contiguous substring of `k` identical characters,
//   concatenating the remaining left and right pieces after each deletion.
//   Continue until no `k` adjacent equal characters remain and return the
//   resulting string (guaranteed to be unique).
//
// Examples:
//   Example 1:
//     Input:  s = "deeedbbcccbdaa", k = 3
//     Output: "aa"
//     Explanation:
//       - Push {d, 1}, then advance 'e' to {e, 3} and immediately pop it when count
//         reaches k = 3, exposing {d, 1}.
//       - Next 'd' merges into {d, 2}, followed by pushing {b, 2} and {c, 3}, where
//         'c' hits k = 3 and pops to expose {b, 2}.
//       - Next 'b' increments {b, 2} to {b, 3} and pops, exposing {d, 2}; the
//         following 'd' increments {d, 2} to {d, 3} and pops.
//       - The final two 'a' characters form {a, 2} on the empty stack, which
//         reconstructs "aa".
//
//   Example 2:
//     Input:  s = "abcd", k = 2
//     Output: "abcd"
//     Explanation:
//       - Each character differs from stack.back().ch, pushing {a, 1}, {b, 1}, {c,
//         1}, and {d, 1}.
//       - No run count reaches k = 2, so no stack entries are popped.
//       - Reconstructing the four remaining entries yields "abcd".
//
//   Example 3:
//     Input:  s = "aaaaa", k = 3
//     Output: "aa"
//     Explanation:
//       - The first three 'a' characters increment the top entry from {a, 1} to {a,
//         3} and pop it at count == 3.
//       - The fourth and fifth 'a' characters push {a, 1} and increment it to {a,
//         2}.
//       - Since count 2 < 3, {a, 2} remains on the stack and reconstructs "aa".
//
// Algorithm:
//   Use a run-length encoded stack that stores `(character, consecutive_count)`
//   pairs. Scan `s` from left to right:
//     1. If the stack is non-empty and its top character matches the current
//        character, increment the top entry's count.
//     2. Whenever the top count reaches `k`, pop the entry — this implicitly
//        merges the newly adjacent left and right characters for subsequent
//        iterations.
//     3. Otherwise, push a new entry `{c, 1}`.
//   Finally, reconstruct the output string by repeating each remaining stack
//   entry's character by its stored count.
//   An equivalent in-place two-pointer variant is also provided, using a
//   parallel count array and rewinding the write cursor by `k` on each match.
//   Time Complexity:  O(n), where n = s.size() (each character is pushed and
//                     popped at most once).
//   Space Complexity: O(n) for the stack (or count array in two-pointer mode).
//
// Data Structures:
//   - Run-length stack (`std::vector<CharRun>`): compresses consecutive runs
//     of identical surviving characters so cascading removals happen in O(1)
//     amortized time per character.
//   - Parallel run-length array (`std::vector<int>` in two-pointer variant):
//     records the consecutive duplicate length ending at each write index.
//
// Real-World Applications:
//   - Run-length compression and stream deduplication filters in network
//     telemetry and log-ingestion pipelines.
//   - Peephole / algebraic cancellation passes in compilers and symbolic math
//     engines (e.g., canceling inverse operations or repeated tokens).
//   - Match-k puzzle engines and text-editor auto-correction / macro collapse.
//
// Related LeetCode Problems:
//   - #1047 Remove All Adjacent Duplicates In String [done] (special case k = 2)
//   - #20 Valid Parentheses [done] (classic stack cancellation invariant)
//   - #1544 Make The String Great [new] (adjacent pair cancellation by case)
//   - #735 Asteroid Collision [done] (stack-driven cascading eliminations)
//   - #1003 Check If Word Is Valid After Substitutions [new] (stack reduction of fixed-length patterns)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

// Represents a contiguous run of identical characters on the reduction stack.
struct CharRun {
  char ch = '\0';
  int count = 0;
};

class Solution {
 public:
  // Removes all k-length adjacent duplicate runs using a run-length stack.
  // Invariant: every entry on `stack` has `1 <= count < k`, and adjacent stack
  // entries always hold distinct characters.
  // Time: O(n), Space: O(n).
  std::string removeDuplicates(std::string_view s, int k) {
    // Guard against degenerate k <= 1 (every character would self-eliminate).
    if (k <= 1) {
      return "";
    }

    std::vector<CharRun> stack;
    stack.reserve(s.size());

    for (char c : s) {
      if (!stack.empty() && stack.back().ch == c) {
        // Extend the active run; pop immediately once it reaches length k so
        // the preceding run becomes exposed for cascading merges.
        ++stack.back().count;
        if (stack.back().count == k) {
          stack.pop_back();
        }
      } else {
        stack.push_back(CharRun{.ch = c, .count = 1});
      }
    }

    // Reconstruct the surviving string in left-to-right order.
    std::string result;
    result.reserve(s.size());
    for (const auto& [ch, count] : stack) {
      result.append(static_cast<std::size_t>(count), ch);
    }
    return result;
  }

  // Two-pointer in-place alternative from the classic optimal solution.
  // Maintains a write cursor `write` over `s` and a parallel `counts` array
  // storing the run length ending at each written position.
  // Time: O(n), Space: O(n).
  std::string removeDuplicatesTwoPointers(std::string s, int k) {
    if (k <= 1) {
      return "";
    }

    const std::size_t n = s.size();
    const auto k_len = static_cast<std::size_t>(k);
    std::vector<int> counts(n, 0);
    std::size_t write = 0;

    for (std::size_t read = 0; read < n; ++read) {
      // Copy next input character into the active prefix of `s`.
      s[write] = s[read];
      counts[write] =
          (write > 0 && s[write - 1] == s[write]) ? (counts[write - 1] + 1) : 1;
      ++write;

      // Once a run of length k completes at `write - 1`, rewind `write` by k
      // to erase it and expose the earlier prefix.
      if (counts[write - 1] == k) {
        write -= k_len;
      }
    }

    s.resize(write);
    return s;
  }
};

LC_PROBLEM(1209, "Remove All Adjacent Duplicates In String II", "stack", ctx) {
  auto verify_both = [&](std::string_view input, int k,
                         std::string_view expected) {
    Solution sol;
    ctx.expect_eq(sol.removeDuplicates(input, k), expected);
    ctx.expect_eq(sol.removeDuplicatesTwoPointers(std::string(input), k),
                  expected);
  };

  ctx.test("example_1_no_removals", [&] { verify_both("abcd", 2, "abcd"); });

  ctx.test("example_2_cascading_k3",
           [&] { verify_both("deeedbbcccbdaa", 3, "aa"); });

  ctx.test("example_3_cascading_k2",
           [&] { verify_both("pbbcggttciiippooaais", 2, "ps"); });

  ctx.test("full_collapse_to_empty", [&] {
    verify_both("aabbcc", 2, "");
    verify_both("abccba", 2, "");
    verify_both("aaabbbccc", 3, "");
  });

  ctx.test("single_character_and_large_k", [&] {
    verify_both("z", 2, "z");
    verify_both("aaaa", 5, "aaaa");
    verify_both("aaaaa", 5, "");
  });

  ctx.test("run_longer_than_k_leaves_remainder", [&] {
    // 5 'a's with k = 3 removes one block of 3, leaving "aa".
    verify_both("aaaaa", 3, "aa");
    // 6 'a's with k = 3 removes two blocks of 3, leaving "".
    verify_both("aaaaaa", 3, "");
  });
}

}  // namespace
