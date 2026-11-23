// LeetCode 127: Word Ladder
// https://leetcode.com/problems/word-ladder/
//
// Problem:
//   Given two equal-length lowercase words `beginWord` and `endWord` and a
//   dictionary `wordList`, return the number of words in the shortest
//   transformation sequence from `beginWord` to `endWord` where each adjacent
//   pair differs by exactly one letter and every transformed word belongs to
//   `wordList`. Return 0 if no valid sequence exists.
//
// Algorithm:
//   Because every transformation step has uniform weight 1, the shortest
//   sequence length corresponds to the minimum hop count in an unweighted
//   graph and is found by Breadth-First Search (BFS). We provide two classic
//   BFS formulations matching the Python source:
//   1. Character-mutation BFS (`ladderLength`): Put `wordList` into an
//      `unvisited` hash set. Expand words level by level; for each word, try
//      mutating every position to `'a'..'z'`. Erasing a matched word from
//      `unvisited` immediately marks it visited (the first level to reach any
//      word is guaranteed to be shortest, so later levels never need it).
//      Return `length + 1` the instant `endWord` is discovered.
//   2. Wildcard-bucket BFS (`ladderLengthWildcard`): Pre-index `wordList` by
//      single-character wildcard masks (e.g., `"hot"` -> `"*ot"`, `"h*t"`,
//      `"ho*"`) so neighbors are fetched directly without trying all 26
//      letters. Clearing each wildcard bucket once expanded guarantees each
//      edge is traversed at most once.
//   Time Complexity:  O(26 * L^2 * N) for mutation BFS; O(L^2 * N) for
//                     wildcard-bucket BFS, where N = |wordList| and L = word
//                     length.
//   Space Complexity: O(N * L) for mutation BFS; O(N * L^2) for wildcard map.
//
// Data Structures:
//   - `std::unordered_set<std::string>`: O(1) average lookup/removal of
//     unvisited dictionary words.
//   - `std::queue<std::string>`: FIFO queue driving level-order BFS expansion.
//   - `std::unordered_map<std::string, std::vector<std::string>>`: Wildcard
//     adjacency buckets in `ladderLengthWildcard`.
//
// Real-World Applications:
//   - Shortest edit-path discovery in fuzzy spell-checkers and autocorrect
//     suggestion engines over a fixed vocabulary.
//   - Point-mutation distance computation in bioinformatics across observed
//     viral or protein k-mer variants.
//   - Minimum-step state-transition search in protocol verification and
//     automated test-case generation.
//
// Related LeetCode Problems:
//   - #126 Word Ladder II [done] (returns all shortest transformation paths)
//   - #433 Minimum Genetic Mutation [new] (same BFS over 8-char DNA strings)
//   - #752 Open the Lock [done] (shortest-path BFS over 4-digit dial states)
//   - #2059 Minimum Operations to Convert Number [new] (unweighted state BFS)

#include "framework/lc_framework.h"

#include <cstddef>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Finds the shortest transformation sequence length using level-order BFS
  // with single-character alphabet mutations.
  // Invariant: Every word dequeued at iteration `length` has minimum distance
  // `length` (counting nodes) from `begin_word`.
  // Time: O(26 * L^2 * N), Space: O(N * L).
  int ladderLength(std::string_view begin_word, std::string_view end_word,
                   const std::vector<std::string>& word_list) {
    std::unordered_set<std::string> unvisited(word_list.begin(),
                                              word_list.end());
    if (!unvisited.contains(std::string(end_word))) {
      return 0;
    }

    unvisited.erase(std::string(begin_word));
    std::queue<std::string> frontier;
    frontier.emplace(begin_word);

    int length = 1;
    while (!frontier.empty()) {
      const std::size_t level_size = frontier.size();
      for (std::size_t step = 0uz; step < level_size; ++step) {
        std::string word = std::move(frontier.front());
        frontier.pop();

        if (expand_mutations(word, end_word, unvisited, frontier)) {
          return length + 1;
        }
      }
      ++length;
    }

    return 0;
  }

  // Alternative BFS using wildcard-pattern adjacency buckets (e.g., "h*t").
  // Avoids testing absent characters and clears each bucket once expanded.
  // Time: O(N * L^2), Space: O(N * L^2).
  int ladderLengthWildcard(std::string_view begin_word,
                           std::string_view end_word,
                           const std::vector<std::string>& word_list) {
    std::unordered_set<std::string> dict(word_list.begin(), word_list.end());
    if (!dict.contains(std::string(end_word))) {
      return 0;
    }

    std::unordered_map<std::string, std::vector<std::string>> buckets;
    for (const std::string& word : dict) {
      std::string pattern = word;
      for (std::size_t i = 0uz; i < pattern.size(); ++i) {
        const char original = pattern[i];
        pattern[i] = '*';
        buckets[pattern].push_back(word);
        pattern[i] = original;
      }
    }

    std::unordered_set<std::string> visited = {std::string(begin_word)};
    std::queue<std::string> frontier;
    frontier.emplace(begin_word);

    int length = 1;
    while (!frontier.empty()) {
      const std::size_t level_size = frontier.size();
      for (std::size_t step = 0uz; step < level_size; ++step) {
        std::string word = std::move(frontier.front());
        frontier.pop();

        if (expand_wildcards(word, end_word, buckets, visited, frontier)) {
          return length + 1;
        }
      }
      ++length;
    }

    return 0;
  }

 private:
  // Mutates each index of `word` across `'a'..'z'`, enqueuing unvisited
  // dictionary matches. Returns true immediately if `end_word` is reached.
  static bool expand_mutations(std::string& word, std::string_view end_word,
                               std::unordered_set<std::string>& unvisited,
                               std::queue<std::string>& frontier) {
    for (std::size_t i = 0uz; i < word.size(); ++i) {
      const char original = word[i];
      for (char ch = 'a'; ch <= 'z'; ++ch) {
        if (ch == original) {
          continue;
        }
        word[i] = ch;
        if (unvisited.erase(word) == 0uz) {
          continue;
        }
        if (word == end_word) {
          return true;
        }
        frontier.push(word);
      }
      word[i] = original;
    }
    return false;
  }

  // Expands all wildcard patterns of `word`, clearing consumed buckets to
  // guarantee linear edge traversal. Returns true iff `end_word` is reached.
  static bool expand_wildcards(
      std::string& word, std::string_view end_word,
      std::unordered_map<std::string, std::vector<std::string>>& buckets,
      std::unordered_set<std::string>& visited,
      std::queue<std::string>& frontier) {
    for (std::size_t i = 0uz; i < word.size(); ++i) {
      const char original = word[i];
      word[i] = '*';

      const auto it = buckets.find(word);
      if (it != buckets.end()) {
        for (const std::string& neighbor : it->second) {
          if (neighbor == end_word) {
            return true;
          }
          if (visited.insert(neighbor).second) {
            frontier.push(neighbor);
          }
        }
        // Every word in this wildcard bucket is now discovered; clear to avoid
        // rescanning the same bucket from future siblings.
        it->second.clear();
      }

      word[i] = original;
    }
    return false;
  }
};

LC_PROBLEM(127, "Word Ladder", "bfs", ctx) {
  Solution sol;

  ctx.test("example_1_shortest_length_5", [&] {
    const std::vector<std::string> words = {"hot", "dot", "dog",
                                            "lot", "log", "cog"};
    ctx.expect_eq(sol.ladderLength("hit", "cog", words), 5);
    ctx.expect_eq(sol.ladderLengthWildcard("hit", "cog", words), 5);
  });

  ctx.test("example_2_end_word_absent", [&] {
    const std::vector<std::string> words = {"hot", "dot", "dog", "lot", "log"};
    ctx.expect_eq(sol.ladderLength("hit", "cog", words), 0);
    ctx.expect_eq(sol.ladderLengthWildcard("hit", "cog", words), 0);
  });

  ctx.test("direct_one_letter_step", [&] {
    const std::vector<std::string> words = {"a", "b", "c"};
    ctx.expect_eq(sol.ladderLength("a", "c", words), 2);
    ctx.expect_eq(sol.ladderLengthWildcard("a", "c", words), 2);
  });

  ctx.test("disconnected_words", [&] {
    const std::vector<std::string> words = {"hot", "dog"};
    ctx.expect_eq(sol.ladderLength("hot", "dog", words), 0);
    ctx.expect_eq(sol.ladderLengthWildcard("hot", "dog", words), 0);
  });

  ctx.test("shortcut_branch_beats_longer_path", [&] {
    const std::vector<std::string> words = {"hot", "dot", "dog", "lot",
                                            "log", "cog", "hog"};
    // hit -> hot -> hog -> cog has length 4.
    ctx.expect_eq(sol.ladderLength("hit", "cog", words), 4);
    ctx.expect_eq(sol.ladderLengthWildcard("hit", "cog", words), 4);
  });
}

}  // namespace
