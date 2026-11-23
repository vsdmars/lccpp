// LeetCode 691: Stickers To Spell Word
// https://leetcode.com/problems/stickers-to-spell-word/
//
// Problem:
//   Given `n` types of stickers (each a lowercase English string available in
//   infinite supply) and a `target` string of length 1..15, cut individual
//   letters from stickers and rearrange them to spell `target`. Return the
//   minimum number of stickers required, or -1 if `target` cannot be formed.
//
// Algorithm:
//   Top-down DFS with memoization over canonical (sorted) remaining target
//   strings. First, compress each sticker into a 26-letter frequency vector
//   restricted to characters present in `target`, discard useless stickers, and
//   prune dominated stickers (any sticker whose per-letter counts are all <=
//   another distinct sticker's counts). An early coverage check returns -1 if
//   any target character is absent from all stickers. During recursion, break
//   symmetry by only trying stickers that contain `remaining[0]` (the smallest
//   remaining character), which guarantees strict progress and explores every
//   optimal multiset of stickers without redundant permutations.
//   Time complexity: O(S * 2^T * T) worst-case (much smaller in practice due to
//   canonical multiset memoization, dominance pruning, and first-character
//   branching), where S is the number of stickers and T = target.size().
//   Space complexity: O(2^T * T) for the memoization table and recursion stack.
//
// Data Structures:
//   - std::array<int, 26>: Fixed-size letter frequency histograms for stickers
//     and the current remaining target string.
//   - std::vector<std::array<int, 26>>: Filtered, dominance-pruned sticker pool
//     for cache-friendly iteration.
//   - std::unordered_map<std::string, int>: Memoization table mapping each
//     canonical sorted remaining string to its minimum sticker count.
//
// Real-World Applications:
//   - Resource bundle / package selection (covering required capabilities or
//     components with the minimum number of pre-packaged kits or SKUs).
//   - Inventory cutting-stock and bill-of-materials optimization in
//     manufacturing and warehouse fulfillment.
//   - Minimum set-multicover problems in cloud container image layering and
//     feature-flag bundle minimization.
//
// Related LeetCode Problems:
//   - #322 Coin Change [done] (unbounded minimum-count cover over a 1D scalar target)
//   - #638 Shopping Offers [done] (unbounded bundle selection with memoized DFS)
//   - #1125 Smallest Sufficient Team [new] (exact minimum set cover via bitmask DP)
//   - #474 Ones and Zeroes [done] (2D multiset resource-count knapsack DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26;
constexpr int kInf = std::numeric_limits<int>::max() / 2;

using FreqMap = std::array<int, kAlphabetSize>;

class Solution {
 public:
  // Computes the minimum number of stickers needed to spell `target`, or -1 if
  // impossible.
  //
  // Invariant: Every memoized key is sorted in ascending alphabetical order, so
  // identical remaining letter multisets share one subproblem entry.
  // Time: O(S * 2^T * T) worst-case; Space: O(2^T * T).
  int minStickers(const std::vector<std::string>& stickers,
                  std::string_view target) {
    if (target.empty()) {
      return 0;
    }

    // Build frequency table for the target and canonicalize `target` by sorting
    // its characters so the initial state matches all recursive state keys.
    const FreqMap target_freq = countLetters(target);
    std::string sorted_target(target);
    std::ranges::sort(sorted_target);

    // Filter stickers to only target-relevant letters and prune dominated ones.
    const std::vector<FreqMap> useful =
        buildUsefulStickers(stickers, target_freq);
    if (!canCoverAllLetters(useful, target_freq)) {
      return -1;
    }

    // Memoization map: canonical remaining string -> minimum stickers needed.
    std::unordered_map<std::string, int> memo;
    memo.emplace("", 0);

    // Recursive top-down solver using C++23 deducing-this lambda.
    auto dfs = [&](this auto&& self, const std::string& remaining) -> int {
      if (const auto it = memo.find(remaining); it != memo.end()) {
        return it->second;
      }

      const FreqMap rem_freq = countLetters(remaining);
      // Symmetry breaking: any valid cover must eventually cover remaining[0],
      // so restrict branching to stickers that supply that specific character.
      const auto pivot = static_cast<std::size_t>(remaining[0] - 'a');

      int best = kInf;
      for (const FreqMap& sticker : useful) {
        if (sticker[pivot] == 0) {
          continue;
        }
        const std::string next_rem = subtractSticker(rem_freq, sticker);
        const int sub = self(next_rem);
        if (sub != kInf) {
          best = std::min(best, sub + 1);
        }
      }

      memo.emplace(remaining, best);
      return best;
    };

    const int ans = dfs(sorted_target);
    return ans == kInf ? -1 : ans;
  }

 private:
  // Counts occurrences of each lowercase English letter in `word`.
  static FreqMap countLetters(std::string_view word) {
    FreqMap freq{};
    for (const char ch : word) {
      ++freq[static_cast<std::size_t>(ch - 'a')];
    }
    return freq;
  }

  // Returns true if `dominator` supplies at least as many copies of every
  // letter as `candidate`.
  static bool dominates(const FreqMap& dominator, const FreqMap& candidate) {
    for (std::size_t i = 0uz; i < kAlphabetSize; ++i) {
      if (dominator[i] < candidate[i]) {
        return false;
      }
    }
    return true;
  }

  // Projects each sticker onto letters present in `target_freq`, drops empty
  // stickers, and removes any sticker strictly or equally dominated by an
  // earlier distinct candidate.
  static std::vector<FreqMap> buildUsefulStickers(
      const std::vector<std::string>& stickers, const FreqMap& target_freq) {
    std::vector<FreqMap> candidates;
    candidates.reserve(stickers.size());

    for (const std::string& word : stickers) {
      FreqMap freq{};
      bool has_overlap = false;
      for (const char ch : word) {
        const auto idx = static_cast<std::size_t>(ch - 'a');
        if (target_freq[idx] > 0) {
          ++freq[idx];
          has_overlap = true;
        }
      }
      if (has_overlap) {
        candidates.push_back(freq);
      }
    }

    // Prune dominated stickers: keep `candidates[i]` only if no other kept or
    // later sticker dominates it (breaking ties on duplicates by index).
    std::vector<FreqMap> pruned;
    pruned.reserve(candidates.size());
    for (std::size_t i = 0uz; i < candidates.size(); ++i) {
      bool is_dominated = false;
      for (std::size_t j = 0uz; j < candidates.size(); ++j) {
        if (i == j) {
          continue;
        }
        if (dominates(candidates[j], candidates[i]) &&
            (candidates[j] != candidates[i] || j < i)) {
          is_dominated = true;
          break;
        }
      }
      if (!is_dominated) {
        pruned.push_back(candidates[i]);
      }
    }
    return pruned;
  }

  // Verifies that every letter required by `target_freq` appears in at least
  // one sticker in `useful`.
  static bool canCoverAllLetters(const std::vector<FreqMap>& useful,
                                 const FreqMap& target_freq) {
    for (std::size_t i = 0uz; i < kAlphabetSize; ++i) {
      if (target_freq[i] == 0) {
        continue;
      }
      bool covered = false;
      for (const FreqMap& sticker : useful) {
        if (sticker[i] > 0) {
          covered = true;
          break;
        }
      }
      if (!covered) {
        return false;
      }
    }
    return true;
  }

  // Subtracts `sticker` letter counts from `rem_freq` and emits the leftover
  // letters in canonical ascending alphabetical order.
  static std::string subtractSticker(const FreqMap& rem_freq,
                                     const FreqMap& sticker) {
    std::string next_rem;
    for (std::size_t i = 0uz; i < kAlphabetSize; ++i) {
      const int left = rem_freq[i] - sticker[i];
      if (left > 0) {
        next_rem.append(static_cast<std::size_t>(left),
                        static_cast<char>('a' + static_cast<int>(i)));
      }
    }
    return next_rem;
  }
};

LC_PROBLEM(691, "Stickers To Spell Word", "dp", ctx) {
  Solution sol;

  ctx.test("example_1_thehat", [&] {
    const std::vector<std::string> stickers = {"with", "example", "science"};
    ctx.expect_eq(sol.minStickers(stickers, "thehat"), 3);
  });

  ctx.test("example_2_impossible", [&] {
    const std::vector<std::string> stickers = {"notice", "possible"};
    ctx.expect_eq(sol.minStickers(stickers, "basicbasic"), -1);
  });

  ctx.test("python_build_case", [&] {
    const std::vector<std::string> stickers = {"with", "example", "science"};
    ctx.expect_eq(sol.minStickers(stickers, "theeeehassst"), 6);
  });

  ctx.test("single_sticker_reused", [&] {
    const std::vector<std::string> stickers = {"ab"};
    ctx.expect_eq(sol.minStickers(stickers, "aaabbb"), 3);
  });

  ctx.test("dominated_and_duplicate_stickers", [&] {
    const std::vector<std::string> stickers = {"a", "ab", "ab", "abc", "xyz"};
    ctx.expect_eq(sol.minStickers(stickers, "aabbcc"), 2);
  });

  ctx.test("empty_inputs", [&] {
    ctx.expect_eq(sol.minStickers({}, "abc"), -1);
    ctx.expect_eq(sol.minStickers({"abc"}, ""), 0);
  });
}

}  // namespace
