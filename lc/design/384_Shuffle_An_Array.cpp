// LeetCode 384: Shuffle An Array
// https://leetcode.com/problems/shuffle-an-array/
//
// Problem:
//   Given an integer array `nums`, design a class `Solution` with:
//     - `Solution(nums)`: initializes the object with the array `nums`.
//     - `reset()`: restores and returns the original configuration of `nums`.
//     - `shuffle()`: returns a uniformly random permutation of `nums` where
//       every one of the n! permutations is equally likely.
//
// Algorithm:
//   Preserve an immutable copy `original_` for O(n) `reset()`. For `shuffle()`,
//   clone `original_` into a working copy and apply the in-place Fisher-Yates
//   (Knuth) shuffle from the end of the array down to index 1:
//     For `k = n - 1` down to `1`:
//       pick a uniform random index `j` in `[0, k]` and swap `arr[k]` with
//       `arr[j]`.
//   Induction invariant: after placing index `k`, the suffix `arr[k..n-1]` is
//   a uniformly chosen `(n - k)`-permutation from the `n` elements (probability
//   `1 / (n * (n - 1) * ... * (k + 1))`), so the completed array achieves an
//   exact `1 / n!` distribution without bias.
//   Time Complexity: O(n) for `reset()` and `shuffle()`.
//   Space Complexity: O(n) to retain the original array and build the output.
//
// Data Structures:
//   - `std::vector<int>` (`original_`): immutable snapshot of the initial
//     element ordering.
//   - `std::mt19937` + `std::uniform_int_distribution<std::size_t>`: unbiased
//     random index generator avoiding modulo bias.
//
// Real-World Applications:
//   - Card-deck shuffling in online games and randomized playlist playback.
//   - Mini-batch shuffling between epochs in stochastic gradient descent (SGD)
//     machine-learning training pipelines.
//   - Randomized A/B experiment assignment and permutation hypothesis testing.
//
// Related LeetCode Problems:
//   - #382 Linked List Random Node [done] (reservoir sampling on streams)
//   - #398 Random Pick Index [done] (uniform index selection via reservoir)
//   - #519 Random Flip Matrix [new] (virtual Fisher-Yates on sparse 2D grid)
//   - #380 Insert Delete GetRandom O [done] (1) (uniform sampling with updates)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <map>
#include <random>
#include <utility>
#include <vector>

namespace {

// Fisher-Yates array shuffler with lossless reset to the initial order.
class Solution {
 public:
  // Captures the original array ordering for future resets and shuffles.
  // Time: O(1) (moves ownership), Space: O(n).
  explicit Solution(std::vector<int> nums) : original_(std::move(nums)) {}

  // Returns the array in its original configuration.
  // Time: O(n), Space: O(n).
  [[nodiscard]] std::vector<int> reset() const { return original_; }

  // Returns a uniformly random permutation of `original_` using the backward
  // Fisher-Yates (Knuth) shuffle.
  // Time: O(n), Space: O(n).
  std::vector<int> shuffle() {
    std::vector<int> result = original_;
    // Loop `i` from `n` down to `2` (operating on index `i - 1`) to avoid
    // unsigned underflow on empty or single-element vectors.
    for (std::size_t i = result.size(); i > 1; --i) {
      std::uniform_int_distribution<std::size_t> dist(0, i - 1);
      std::swap(result[i - 1], result[dist(rng_)]);
    }
    return result;
  }

 private:
  std::vector<int> original_;
  std::mt19937 rng_{std::random_device{}()};
};

LC_PROBLEM(384, "Shuffle An Array", "design", ctx) {
  ctx.test("leetcode example [1, 2, 3] permutation and uniformity", [&] {
    const std::vector<int> orig = {1, 2, 3};
    Solution sol(orig);

    std::map<std::vector<int>, int> freq;
    constexpr int kTrials = 6000;
    for (int i = 0; i < kTrials; ++i) {
      std::vector<int> perm = sol.shuffle();
      ++freq[perm];
      std::ranges::sort(perm);
      ctx.expect_eq(perm, orig);
    }

    // All 3! = 6 permutations must appear roughly equally (~1000 times each).
    ctx.expect_eq(freq.size(), 6uz);
    for (const auto& entry : freq) {
      ctx.expect_true(entry.second > 750 && entry.second < 1250);
    }

    // Reset must always restore the exact original order.
    ctx.expect_eq(sol.reset(), orig);
  });

  ctx.test("reset remains intact across repeated shuffles", [&] {
    const std::vector<int> orig = {-5, 0, -5, 10, 42};
    Solution sol(orig);
    std::vector<int> sorted_orig = orig;
    std::ranges::sort(sorted_orig);

    for (int i = 0; i < 20; ++i) {
      std::vector<int> shuffled = sol.shuffle();
      std::ranges::sort(shuffled);
      ctx.expect_eq(shuffled, sorted_orig);
      ctx.expect_eq(sol.reset(), orig);
    }
  });

  ctx.test("edge cases: empty and single-element arrays", [&] {
    Solution empty_sol({});
    ctx.expect_eq(empty_sol.shuffle(), std::vector<int>{});
    ctx.expect_eq(empty_sol.reset(), std::vector<int>{});

    Solution single_sol({7});
    ctx.expect_eq(single_sol.shuffle(), std::vector<int>{7});
    ctx.expect_eq(single_sol.reset(), std::vector<int>{7});
  });
}

}  // namespace
