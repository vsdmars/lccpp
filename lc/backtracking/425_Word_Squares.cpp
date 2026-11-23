// LeetCode 425: Word Squares
// https://leetcode.com/problems/word-squares/
//
// Problem:
//   Given an array of unique strings `words` where every word has the exact
//   same length `L` (1 <= L <= 5, 1 <= words.length <= 1000, lowercase English
//   letters), return all valid word squares that can be built from `words`
//   (words may be reused multiple times).
//   A sequence of `L` words forms a valid word square if for every `0 <= k < L`,
//   the `k`-th row and `k`-th column read the exact same string.
//
// Algorithm:
//   Prefix-Indexed Backtracking:
//   1. Precompute a hash map `prefix_map` mapping every non-empty prefix
//      `word[0..len)` (for `1 <= len < L`) to the list of word indices in
//      `words` sharing that prefix.
//   2. Seed row 0 of the candidate square with each word in `words` and launch
//      backtracking at `row = 1`.
//   3. At recursion step `row`:
//      - Base case: if `row == L`, emit the completed `L x L` word square.
//      - Because the square must be symmetric across its main diagonal, row
//        `row` MUST start with the `row`-character prefix formed by column
//        `row` of rows `0 .. row - 1`: `prefix[r] = square[r][row]`.
//      - Look up `prefix` in `prefix_map`. For every matching candidate word,
//        append it to `square`, recurse to `row + 1`, and pop on backtrack.
//   Time Complexity:  O(N * L) to build the prefix index, plus backtracking
//                     bounded by O(N * 26^L) worst-case (dramatically pruned
//                     in practice by prefix lookup).
//   Space Complexity: O(N * L) for the prefix map (storing zero-copy
//                     `std::string_view` keys and index lists) plus O(L)
//                     recursion depth (excluding output storage).
//
// Data Structures:
//   - Prefix index (`std::unordered_map<std::string_view, std::vector<std::size_t>>`)
//     referencing slices of the input words with zero string duplication.
//   - Mutable candidate square (`std::vector<std::string_view>`) pushed/popped
//     in place and materialized into `std::vector<std::string>` only upon
//     completing all `L` rows.
//
// Real-World Applications:
//   - Crossword puzzle and symmetric word-grid generators solving 2D lexical
//     constraint-satisfaction problems via prefix propagation.
//   - DNA/protein palindromic and symmetric matrix motif discovery in
//     computational biology.
//   - Constraint-satisfaction solvers using forward-checking lookup tables to
//     prune infeasible assignments early.
//
// Related LeetCode Problems:
//   - #422 Valid Word Square [new] (verification version checking an existing grid)
//   - #208 Implement Trie [done] (Prefix Tree) (alternative trie structure for prefix queries)
//   - #212 Word Search II [done] (prefix-pruned backtracking on character grids)
//   - #37 Sudoku Solver [done] (2D row/column constraint-satisfaction backtracking)
//   - #79 Word Search [done] (grid DFS backtracking)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Finds all valid word squares buildable from `words` (with word reuse
  // allowed) using prefix-indexed backtracking.
  // Time: O(N * L) preprocessing + prefix-pruned backtracking;
  // Space: O(N * L) for prefix index + O(L) recursion stack.
  std::vector<std::vector<std::string>> wordSquares(
      const std::vector<std::string>& words) {
    if (words.empty()) {
      return {};
    }

    const std::size_t word_len = words[0].size();
    if (word_len == 0) {
      return {};
    }

    // Build prefix -> list of word indices. Keys are non-owning views into
    // `words`, avoiding heap allocations for every prefix substring.
    std::unordered_map<std::string_view, std::vector<std::size_t>> prefix_map;
    for (std::size_t idx = 0; idx < words.size(); ++idx) {
      const std::string_view view(words[idx]);
      for (std::size_t len = 1; len < word_len; ++len) {
        prefix_map[view.substr(0, len)].push_back(idx);
      }
    }

    std::vector<std::vector<std::string>> results;
    std::vector<std::string_view> square;
    square.reserve(word_len);
    std::string needed_prefix;
    needed_prefix.reserve(word_len);

    // Places row `step` using the prefix dictated by column `step` of rows
    // `0 .. step - 1`.
    auto dfs = [&](this auto&& self, std::size_t step) -> void {
      if (step == word_len) {
        std::vector<std::string> completed;
        completed.reserve(word_len);
        for (const std::string_view row : square) {
          completed.emplace_back(row);
        }
        results.push_back(std::move(completed));
        return;
      }

      // Extract column `step` from the already-placed rows to form the
      // mandatory prefix for row `step`.
      needed_prefix.clear();
      for (std::size_t r = 0; r < step; ++r) {
        needed_prefix.push_back(square[r][step]);
      }

      const auto it = prefix_map.find(needed_prefix);
      if (it == prefix_map.end()) {
        return;
      }

      for (const std::size_t cand_idx : it->second) {
        square.push_back(words[cand_idx]);
        self(step + 1);
        square.pop_back();
      }
    };

    // Every word can serve as row 0 of a candidate word square.
    for (const auto& word : words) {
      square.push_back(word);
      dfs(1uz);
      square.pop_back();
    }

    return results;
  }
};

LC_PROBLEM(425, "Word Squares", "backtracking", ctx) {
  Solution sol;

  ctx.test("example_1_two_squares", [&] {
    const std::vector<std::string> words = {
        "area", "lead", "wall", "lady", "ball",
    };
    auto actual = sol.wordSquares(words);
    std::ranges::sort(actual);

    std::vector<std::vector<std::string>> expected = {
        {"ball", "area", "lead", "lady"},
        {"wall", "area", "lead", "lady"},
    };
    std::ranges::sort(expected);
    ctx.expect_eq(actual, expected);
  });

  ctx.test("example_2_with_word_reuse", [&] {
    const std::vector<std::string> words = {"abat", "baba", "atan", "atal"};
    auto actual = sol.wordSquares(words);
    std::ranges::sort(actual);

    std::vector<std::vector<std::string>> expected = {
        {"baba", "abat", "baba", "atal"},
        {"baba", "abat", "baba", "atan"},
    };
    std::ranges::sort(expected);
    ctx.expect_eq(actual, expected);
  });

  ctx.test("python_build_input_single_square", [&] {
    const std::vector<std::string> words = {"ball", "area", "lead", "lady"};
    const std::vector<std::vector<std::string>> expected = {
        {"ball", "area", "lead", "lady"},
    };
    ctx.expect_eq(sol.wordSquares(words), expected);
  });

  ctx.test("single_character_and_no_solution", [&] {
    const std::vector<std::string> single_char = {"a", "b"};
    const std::vector<std::vector<std::string>> expected_single = {
        {"a"},
        {"b"},
    };
    ctx.expect_eq(sol.wordSquares(single_char), expected_single);

    const std::vector<std::string> impossible = {"abc", "def"};
    ctx.expect_true(sol.wordSquares(impossible).empty());
  });
}

}  // namespace
