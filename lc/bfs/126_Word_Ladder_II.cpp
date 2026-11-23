// LeetCode 126: Word Ladder II
// https://leetcode.com/problems/word-ladder-ii/
//
// Problem:
//   Given two equal-length lowercase strings `beginWord` and `endWord` and a
//   dictionary `wordList`, return all shortest transformation sequences from
//   `beginWord` to `endWord` such that:
//     1. Only one character changes between consecutive words.
//     2. Every intermediate word (and `endWord`) exists in `wordList`.
//        (`beginWord` need not be in `wordList`.)
//   Return an empty list if no valid transformation exists.
//
// Algorithm:
//   Two-phase BFS + reverse-DAG DFS backtracking:
//   1. Level-order BFS (`build_parent_dag`): Store all dictionary words in an
//      `unvisited` hash set and expand level by level starting from
//      `beginWord`. For each word in the current frontier, mutate every letter
//      position across `'a'..'z'`. Whenever a mutated candidate is still in
//      `unvisited`, record `word` as a predecessor in `parents[candidate]` and
//      add `candidate` to `next_frontier`. Crucially, erase `next_frontier`
//      from `unvisited` ONLY AFTER the entire current level finishes; this
//      allows multiple shortest-path predecessors at depth `d` to link to the
//      same node at depth `d + 1` while preventing longer/cyclic edges. Stop
//      expanding immediately after the first level that discovers `endWord`.
//   2. Backward DFS (`collect_paths`): Walk backwards from `endWord` to
//      `beginWord` following `parents` edges. Because only nodes on valid
//      shortest paths lead back to `beginWord`, dead-end BFS branches are never
//      traversed. Reverse each completed path to emit `beginWord -> ... ->
//      endWord`.
//   Time Complexity:  O(26 * L^2 * N + P * K), where N = |wordList|,
//                     L = word length, K = shortest path length, and P = number
//                     of shortest paths emitted.
//   Space Complexity: O(N * L + P * K) for the dictionary, parent DAG, and
//                     output paths.
//
// Data Structures:
//   - `std::unordered_set<std::string>`: Tracks unvisited dictionary words and
//     deduplicated BFS frontiers per level.
//   - `std::unordered_map<std::string, std::vector<std::string>>`: Represents
//     the reverse shortest-path DAG (`word -> all shortest-path predecessors`).
//   - `std::vector<std::string>`: Backtracking stack for path reconstruction.
//
// Real-World Applications:
//   - Equal-cost multi-path routing (ECMP) in computer networks where routers
//     maintain a DAG of all shortest next-hops for load balancing.
//   - Spell-checking and DNA/protein sequence edit-path enumeration when all
//     minimum-mutation evolutionary trajectories must be inspected.
//   - Puzzle solvers and state-space planners needing every optimal action
//     sequence rather than an arbitrary single one.
//
// Related LeetCode Problems:
//   - #127 Word Ladder [done] (computes only the shortest transformation length)
//   - #433 Minimum Genetic Mutation [new] (identical BFS on 4-letter alphabet)
//   - #752 Open the Lock [done] (shortest-path BFS on string states)
//   - #797 All Paths From Source to Target [done] (DAG path enumeration via DFS)
//   - #1976 Number of Ways to Arrive at Destination [new] (shortest-path DAG)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

using ParentMap = std::unordered_map<std::string, std::vector<std::string>>;

class Solution {
 public:
  // Finds all shortest one-letter transformation sequences from `begin_word`
  // to `end_word` using words in `word_list`.
  // Time: O(26 * L^2 * N + P * K), Space: O(N * L + P * K).
  std::vector<std::vector<std::string>> findLadders(
      std::string_view begin_word, std::string_view end_word,
      const std::vector<std::string>& word_list) {
    ParentMap parents;
    if (!build_parent_dag(begin_word, end_word, word_list, parents)) {
      return {};
    }

    // Reconstruct every shortest path by backtracking from end_word to
    // begin_word so dead-end BFS branches are skipped completely.
    std::vector<std::vector<std::string>> results;
    std::vector<std::string> current_path = {std::string(end_word)};
    collect_paths(current_path.back(), begin_word, parents, current_path,
                  results);
    return results;
  }

 private:
  // Generates all valid single-character mutations of `word` present in
  // `unvisited`, recording reverse parent edges and populating `next_frontier`.
  // Returns true if `end_word` is discovered among the neighbors.
  static bool expand_word(const std::string& word, std::string_view end_word,
                          const std::unordered_set<std::string>& unvisited,
                          std::unordered_set<std::string>& next_frontier,
                          ParentMap& parents) {
    bool found_end = false;
    std::string candidate = word;

    for (std::size_t idx = 0uz; idx < candidate.size(); ++idx) {
      const char original = candidate[idx];
      for (char ch = 'a'; ch <= 'z'; ++ch) {
        if (ch == original) {
          continue;
        }
        candidate[idx] = ch;
        if (!unvisited.contains(candidate)) {
          continue;
        }
        parents[candidate].push_back(word);
        next_frontier.insert(candidate);
        if (candidate == end_word) {
          found_end = true;
        }
      }
      candidate[idx] = original;
    }

    return found_end;
  }

  // Runs level-order BFS from `begin_word` until the level containing
  // `end_word` finishes, populating the shortest-path predecessor DAG `parents`.
  // Returns true iff `end_word` is reachable.
  static bool build_parent_dag(std::string_view begin_word,
                               std::string_view end_word,
                               const std::vector<std::string>& word_list,
                               ParentMap& parents) {
    std::unordered_set<std::string> unvisited(word_list.begin(),
                                              word_list.end());
    if (!unvisited.contains(std::string(end_word))) {
      return false;
    }

    // Remove begin_word if present so BFS never revisits depth 0.
    unvisited.erase(std::string(begin_word));

    std::unordered_set<std::string> frontier = {std::string(begin_word)};
    bool found_end = false;

    while (!frontier.empty() && !found_end) {
      std::unordered_set<std::string> next_frontier;
      for (const std::string& word : frontier) {
        if (expand_word(word, end_word, unvisited, next_frontier, parents)) {
          found_end = true;
        }
      }

      // Erase all words discovered at this depth ONLY after the full level
      // completes so every shortest-path predecessor at depth d is recorded.
      for (const std::string& visited_word : next_frontier) {
        unvisited.erase(visited_word);
      }
      frontier = std::move(next_frontier);
    }

    return found_end;
  }

  // Recursively backtracks from `word` toward `begin_word` along `parents`
  // edges, emitting each reversed path into `results`.
  static void collect_paths(const std::string& word,
                            std::string_view begin_word,
                            const ParentMap& parents,
                            std::vector<std::string>& current_path,
                            std::vector<std::vector<std::string>>& results) {
    if (word == begin_word) {
      std::vector<std::string> forward_path = current_path;
      std::ranges::reverse(forward_path);
      results.push_back(std::move(forward_path));
      return;
    }

    const auto it = parents.find(word);
    if (it == parents.end()) {
      return;
    }

    for (const std::string& parent : it->second) {
      current_path.push_back(parent);
      collect_paths(parent, begin_word, parents, current_path, results);
      current_path.pop_back();
    }
  }
};

LC_PROBLEM(126, "Word Ladder II", "bfs", ctx) {
  Solution sol;

  ctx.test("example_1_two_shortest_paths", [&] {
    const std::vector<std::string> words = {"hot", "dot", "dog",
                                            "lot", "log", "cog"};
    auto actual = sol.findLadders("hit", "cog", words);
    std::ranges::sort(actual);
    const std::vector<std::vector<std::string>> expected = {
        {"hit", "hot", "dot", "dog", "cog"},
        {"hit", "hot", "lot", "log", "cog"},
    };
    ctx.expect_eq(actual, expected);
  });

  ctx.test("example_2_end_word_missing", [&] {
    const std::vector<std::string> words = {"hot", "dot", "dog", "lot", "log"};
    const auto actual = sol.findLadders("hit", "cog", words);
    ctx.expect_true(actual.empty());
  });

  ctx.test("single_step_direct_ladder", [&] {
    const std::vector<std::string> words = {"a", "b", "c"};
    const auto actual = sol.findLadders("a", "c", words);
    const std::vector<std::vector<std::string>> expected = {{"a", "c"}};
    ctx.expect_eq(actual, expected);
  });

  ctx.test("multiple_converging_diamond_paths", [&] {
    const std::vector<std::string> words = {"ted", "tex", "red", "tax",
                                            "tad", "den", "rex", "pee"};
    auto actual = sol.findLadders("red", "tax", words);
    std::ranges::sort(actual);
    const std::vector<std::vector<std::string>> expected = {
        {"red", "rex", "tex", "tax"},
        {"red", "ted", "tad", "tax"},
        {"red", "ted", "tex", "tax"},
    };
    ctx.expect_eq(actual, expected);
  });

  ctx.test("disconnected_dictionary", [&] {
    const std::vector<std::string> words = {"hot", "dog"};
    const auto actual = sol.findLadders("hot", "dog", words);
    ctx.expect_true(actual.empty());
  });
}

}  // namespace
