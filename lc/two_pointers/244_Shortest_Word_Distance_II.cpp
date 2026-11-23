// LeetCode 244: Shortest Word Distance II
// https://leetcode.com/problems/shortest-word-distance-ii/
//
// Problem:
//   Design a data structure `WordDistance` initialized with an array of
//   strings `wordsDict` that repeatedly answers queries `shortest(word1,
//   word2)`, returning the minimum index distance `|i - j|` between an
//   occurrence of `word1` and an occurrence of `word2` in `wordsDict`.
//   `word1` and `word2` are guaranteed to be distinct and present in
//   `wordsDict`.
//
// Algorithm:
//   Precompute an inverted index in the constructor mapping each unique word
//   to the list of positions where it appears. Because we scan `wordsDict`
//   from index `0` to `n - 1`, every word's position list is automatically
//   sorted in strictly increasing order.
//   To answer `shortest(word1, word2)`, run a two-pointer merge walk over the
//   two sorted posting lists `idx1` and `idx2`:
//     - Compare `idx1[i]` and `idx2[j]`, update `min_dist` with their gap,
//       and advance whichever pointer points to the smaller index (advancing
//       the larger index could only increase the distance to the smaller one).
//     - Return early if `min_dist == 1`, since distinct words cannot occupy
//       the same index.
//   Time Complexity:  O(n) construction; O(k1 + k2) per query, where k1 and k2
//                     are the occurrence counts of `word1` and `word2`.
//   Space Complexity: O(n) to store all word indices across the inverted index.
//
// Data Structures:
//   Inverted index `std::unordered_map<std::string, std::vector<int>>` storing
//   strictly sorted occurrence lists per word, queried with two `std::size_t`
//   merge pointers.
//
// Real-World Applications:
//   - Full-text search engines (Lucene, Elasticsearch) evaluating proximity
//     queries (`"term1 term2"~k`) by intersecting sorted positional posting
//     lists.
//   - NLP and corpus linguistics computing minimum co-occurrence spans between
//     terms across large documents.
//   - Log correlation engines measuring the tightest time/sequence gap between
//     two recurring event types from indexed event timestamps.
//
// Related LeetCode Problems:
//   - #243 Shortest Word Distance [done] (single-query O(n) two-pointer scan without preprocessing)
//   - #245 Shortest Word Distance III [new] (allows `word1 == word2`)
//   - #986 Interval List Intersections [done] (two-pointer merge walk over sorted lists)
//   - #350 Intersection of Two Arrays II [done] (two-pointer merge over sorted sequences)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

// Preprocesses a word sequence into a positional inverted index to support
// repeated shortest-distance queries between distinct words.
class WordDistance {
 public:
  // Indexes every occurrence of every word in `words_dict`.
  //
  // Complexity: O(n) time and space, where n = words_dict.size().
  explicit WordDistance(const std::vector<std::string>& words_dict) {
    for (std::size_t i = 0uz; i < words_dict.size(); ++i) {
      // Scanning left-to-right guarantees each word's index vector is strictly
      // increasing without needing a subsequent sort.
      positions_[words_dict[i]].push_back(static_cast<int>(i));
    }
  }

  // Returns the minimum index distance between any occurrence of `word1` and
  // any occurrence of `word2`.
  //
  // Invariant: at each step of the two-pointer walk, all pairs involving an
  // already-advanced pointer have had their closest possible partner checked.
  //
  // Complexity: O(k1 + k2) time, O(1) extra space.
  [[nodiscard]] int shortest(const std::string& word1,
                             const std::string& word2) const {
    const auto& idx1 = positions_.at(word1);
    const auto& idx2 = positions_.at(word2);

    std::size_t i = 0uz;
    std::size_t j = 0uz;
    int min_dist = std::numeric_limits<int>::max();

    while (i < idx1.size() && j < idx2.size()) {
      if (idx1[i] < idx2[j]) {
        min_dist = std::min(min_dist, idx2[j] - idx1[i]);
        ++i;
      } else {
        min_dist = std::min(min_dist, idx1[i] - idx2[j]);
        ++j;
      }

      // Because `word1 != word2`, indices are always distinct, so 1 is the
      // smallest distance achievable.
      if (min_dist == 1) {
        return 1;
      }
    }

    return min_dist;
  }

 private:
  std::unordered_map<std::string, std::vector<int>> positions_;
};

LC_PROBLEM(244, "Shortest Word Distance II", "two_pointers", ctx) {
  ctx.test("leetcode_example_1_and_python_build", [&] {
    const WordDistance wd(
        {"practice", "makes", "perfect", "coding", "makes"});
    ctx.expect_eq(wd.shortest("coding", "practice"), 3);
    ctx.expect_eq(wd.shortest("makes", "coding"), 1);
    // Symmetry and additional pairs on the same instance.
    ctx.expect_eq(wd.shortest("practice", "coding"), 3);
    ctx.expect_eq(wd.shortest("coding", "makes"), 1);
    ctx.expect_eq(wd.shortest("practice", "makes"), 1);
    ctx.expect_eq(wd.shortest("perfect", "makes"), 1);
  });

  ctx.test("interleaved_repeated_occurrences", [&] {
    const WordDistance wd({"a", "x", "x", "b", "c", "a", "b"});
    ctx.expect_eq(wd.shortest("a", "b"), 1);
    ctx.expect_eq(wd.shortest("a", "c"), 1);
    ctx.expect_eq(wd.shortest("x", "b"), 1);
    ctx.expect_eq(wd.shortest("x", "c"), 2);
  });

  ctx.test("distant_endpoints", [&] {
    const WordDistance wd({"start", "mid", "mid", "mid", "mid", "end"});
    ctx.expect_eq(wd.shortest("start", "end"), 5);
    ctx.expect_eq(wd.shortest("end", "start"), 5);
  });

  ctx.test("two_element_dictionary", [&] {
    const WordDistance wd({"alpha", "beta"});
    ctx.expect_eq(wd.shortest("alpha", "beta"), 1);
    ctx.expect_eq(wd.shortest("beta", "alpha"), 1);
  });
}

}  // namespace
