// LeetCode 243: Shortest Word Distance
// https://leetcode.com/problems/shortest-word-distance/
//
// Problem:
//   Given an array of strings `words_dict` and two distinct strings `word1` and
//   `word2` that are guaranteed to appear in `words_dict`, return the shortest
//   index distance `|i - j|` between any occurrence of `word1` at index `i` and
//   any occurrence of `word2` at index `j`.
//
// Examples:
//   Example 1:
//     Input:  words_dict = ["practice", "makes", "perfect", "coding", "makes"],
//             word1 = "coding", word2 = "practice"
//     Output: 3
//     Explanation:
//       - Initialize min_dist = 5, last_word = "", and last_idx = nullopt.
//       - At idx = 0 ("practice"), last_idx is empty, so set last_word = "practice"
//         and last_idx = 0.
//       - Skip idx = 1 ("makes") and idx = 2 ("perfect") because neither matches
//         word1 or word2.
//       - At idx = 3 ("coding"), word differs from last_word ("practice"), so
//         update min_dist = min(5, 3 - 0) = 3 and set last_word = "coding",
//         last_idx = 3.
//       - Skip idx = 4 ("makes") and return min_dist = 3.
//
//   Example 2:
//     Input:  words_dict = ["a", "x", "y", "b", "z", "a", "b"], word1 = "a", word2
//             = "b"
//     Output: 1
//     Explanation:
//       - Initialize min_dist = 7, last_word = "", and last_idx = nullopt; at idx =
//         0 ("a"), set last_word = "a" and last_idx = 0.
//       - Skip "x" (idx = 1) and "y" (idx = 2), then at idx = 3 ("b") update
//         min_dist = min(7, 3 - 0) = 3 and set last_word = "b", last_idx = 3.
//       - Skip "z" (idx = 4), then at idx = 5 ("a") update min_dist = min(3, 5 - 3)
//         = 2 and set last_word = "a", last_idx = 5.
//       - At idx = 6 ("b"), word differs from last_word ("a"), so update min_dist =
//         min(2, 6 - 5) = 1 and return 1.
//
//   Example 3:
//     Input:  words_dict = ["alpha", "beta"], word1 = "beta", word2 = "alpha"
//     Output: 1
//     Explanation:
//       - Initialize min_dist = 2, last_word = "", and last_idx = nullopt.
//       - At idx = 0 ("alpha"), record last_word = "alpha" and last_idx = 0.
//       - At idx = 1 ("beta"), word differs from last_word ("alpha"), so update
//         min_dist = min(2, 1 - 0) = 1 and return 1.
//
// Algorithm:
//   Single-pass linear scan tracking only the most recently encountered target
//   word (`last_word`) and its index (`last_idx`):
//   - Ignore any word that matches neither `word1` nor `word2`.
//   - Whenever a target word is seen and it differs from `last_word` (meaning
//     we just saw the opposite target word most recently), update the minimum
//     distance with `idx - *last_idx`.
//   - Always update `last_word` and `last_idx` to the current occurrence,
//     because any earlier occurrence of the same word can only produce a larger
//     distance to future occurrences of the other word.
//   Time Complexity:  O(n * L), where n is the number of words and L is the
//                     average compared word length.
//   Space Complexity: O(1) extra space (stores only a `std::string_view` and
//                     an index).
//
// Data Structures:
//   - `std::string_view` (`last_word`): non-owning reference to the most
//     recently matched target word, avoiding string copies.
//   - `std::optional<std::size_t>` (`last_idx`): index of that most recent
//     match (empty before either target word is seen).
//
// Real-World Applications:
//   - Full-text search engines implementing proximity queries (e.g., Lucene /
//     Elasticsearch `NEAR` or phrase-slop scoring between two query terms).
//   - Log and event-stream correlation (finding the tightest gap between a
//     trigger event and an error/recovery event in an audit trail).
//   - Bioinformatics motif proximity analysis (computing minimum base/token
//     spacing between two regulatory sequence markers).
//
// Related LeetCode Problems:
//   - #244 Shortest Word Distance II [done] (multiple queries using hash map of index lists)
//   - #245 Shortest Word Distance III [new] (extension where `word1` and `word2` may be equal)
//   - #76 Minimum Window Substring [done] (minimum span containing target tokens)
//   - #2260 Minimum Consecutive Cards to Pick Up [new] (minimum distance between matching values)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum index distance between any occurrence of `word1` and
  // any occurrence of `word2` in `words_dict`.
  //
  // Key Invariant:
  //   At every step, `last_idx` holds the rightmost occurrence seen so far of
  //   either target word (`last_word`). Because a closer pair can only form
  //   between adjacent alternating occurrences of `word1` and `word2`, checking
  //   `word != last_word` is both necessary and sufficient to find the global
  //   minimum distance.
  //
  // Time Complexity:  O(n * L)
  // Space Complexity: O(1)
  int shortestDistance(const std::vector<std::string>& words_dict,
                       std::string_view word1, std::string_view word2) {
    std::size_t min_dist = words_dict.size();
    std::string_view last_word;
    std::optional<std::size_t> last_idx;

    for (std::size_t idx = 0uz; idx < words_dict.size(); ++idx) {
      const std::string_view word = words_dict[idx];
      if (word != word1 && word != word2) {
        continue;
      }

      // If the previously seen target word was the *other* word, their gap is
      // a candidate shortest distance.
      if (last_idx.has_value() && word != last_word) {
        min_dist = std::min(min_dist, idx - *last_idx);
      }

      // Always advance to the latest occurrence so future matches measure from
      // the closest possible position.
      last_word = word;
      last_idx = idx;
    }

    return static_cast<int>(min_dist);
  }
};

LC_PROBLEM(243, "Shortest Word Distance", "array", ctx) {
  Solution sol;

  ctx.test("example_1_and_2", [&] {
    const std::vector<std::string> words = {"practice", "makes", "perfect",
                                            "coding", "makes"};
    ctx.expect_eq(sol.shortestDistance(words, "coding", "practice"), 3);
    ctx.expect_eq(sol.shortestDistance(words, "makes", "coding"), 1);
  });

  ctx.test("python_driver_case", [&] {
    const std::vector<std::string> words = {"perfect", "practice", "makes",
                                            "perfect", "coding",   "makes"};
    ctx.expect_eq(sol.shortestDistance(words, "coding", "practice"), 3);
  });

  ctx.test("multiple_occurrences_tightening", [&] {
    const std::vector<std::string> words = {"a", "x", "y", "b",
                                            "z", "a", "b"};
    ctx.expect_eq(sol.shortestDistance(words, "a", "b"), 1);
  });

  ctx.test("two_element_list", [&] {
    const std::vector<std::string> words = {"alpha", "beta"};
    ctx.expect_eq(sol.shortestDistance(words, "beta", "alpha"), 1);
  });

  ctx.test("words_at_opposite_ends", [&] {
    const std::vector<std::string> words = {"start", "m1", "m2", "m3", "end"};
    ctx.expect_eq(sol.shortestDistance(words, "start", "end"), 4);
  });
}

}  // namespace
