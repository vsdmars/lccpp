// LeetCode 638: Shopping Offers
// https://leetcode.com/problems/shopping-offers/
//
// Problem:
//   You are given `price[i]` (the unit price of item `i`), a list of bundle
//   `special` offers where `special[j] = [q_0, ..., q_{n-1}, bundle_price]`
//   supplies `q_i` units of item `i` for `bundle_price`, and a `needs` array
//   specifying the exact quantity of each item to buy (`n <= 6`, each
//   `needs[i] <= 10`). Each special offer may be used any number of times, but
//   you are never allowed to purchase more of any item than needed. Return the
//   lowest total price to acquire the exact `needs`.
//
// Examples:
//   Example 1:
//     Input:  price = [2, 5], special = [[3, 0, 5], [1, 2, 10]], needs = [3, 2]
//     Output: 14
//     Explanation:
//       - Pre-filtering keeps both offers in useful_offers because 5 < 3*2 + 0*5 =
//         6 for [3, 0, 5] and 10 < 1*2 + 2*5 = 12 for [1, 2, 10].
//       - At dfs([3, 2]), directPurchaseCost sets the baseline best = 3*2 + 2*5 =
//         16.
//       - Applying [3, 0, 5] gives next_needs = [0, 2], where neither offer fits
//         and dfs([0, 2]) returns 2*5 = 10, yielding total 5 + 10 = 15.
//       - Applying [1, 2, 10] gives next_needs = [2, 0], where neither offer fits
//         and dfs([2, 0]) returns 2*2 = 4, yielding total 10 + 4 = 14.
//       - dfs caches memo[[3, 2]] = min(16, 15, 14) = 14 and returns 14.
//
//   Example 2:
//     Input:  price = [2, 3, 4], special = [[1, 1, 0, 4], [2, 2, 1, 9]], needs =
//             [1, 2, 1]
//     Output: 11
//     Explanation:
//       - Pre-filtering retains [1, 1, 0, 4] (4 < 2 + 3 = 5) and [2, 2, 1, 9] (9 <
//         4 + 6 + 4 = 14) in useful_offers.
//       - At dfs([1, 2, 1]), directPurchaseCost computes a baseline of 1*2 + 2*3 +
//         1*4 = 12.
//       - tryApplyOffer rejects [2, 2, 1, 9] because offer[0] = 2 exceeds needs[0]
//         = 1, while [1, 1, 0, 4] succeeds with next_needs = [0, 1, 1].
//       - In dfs([0, 1, 1]), no offer fits (both need at least 1 unit of item 0),
//         so it returns 1*3 + 1*4 = 7, updating best = min(12, 4 + 7) = 11.
//
//   Example 3:
//     Input:  price = [2, 3], special = [[0, 0, 0], [1, 1, 10], [3, 0, 1]], needs =
//             [2, 1]
//     Output: 7
//     Explanation:
//       - Pre-filtering discards [0, 0, 0] (total_items == 0) and [1, 1, 10] (offer
//         price 10 >= direct cost 5), keeping only [3, 0, 1] (1 < 6).
//       - At dfs([2, 1]), directPurchaseCost sets baseline best = 2*2 + 1*3 = 7.
//       - tryApplyOffer rejects [3, 0, 1] since offer[0] = 3 > needs[0] = 2, so no
//         offer applies and dfs returns 7.
//
// Algorithm:
//   First pre-filter `special` to keep only strictly beneficial non-empty
//   offers (those supplying at least one item at a price strictly lower than
//   buying those items individually). This eliminates zero-item offers that
//   would cause infinite recursion and prunes dominated bundles. Then run
//   top-down DFS with memoization keyed by the remaining `needs` vector:
//   for any `curr_needs`, start with the baseline cost of buying all remaining
//   items individually, then try applying each filtered offer that does not
//   exceed `curr_needs` on any item, updating `best = min(best, offer_price +
//   dfs(next_needs))`, and cache `memo[curr_needs] = best`.
//   Time Complexity:  O(S * n * prod(needs[i] + 1)) worst case, where S is the
//                     number of useful offers and n is the number of item types.
//   Space Complexity: O(n * prod(needs[i] + 1)) for the memoization table plus
//                     O(sum(needs)) recursion depth.
//
// Data Structures:
//   - Filtered offer list (`std::vector<std::vector<int>>`): holds only
//     strictly cost-saving, non-empty bundles.
//   - Memoization hash map (`std::unordered_map<std::vector<int>, int, VecHash>`):
//     caches the optimal minimum price for each reachable remaining-needs state.
//
// Real-World Applications:
//   - E-commerce & retail checkout optimization: automatically applying combo
//     deals, meal bundles, and multi-buy coupons to minimize basket total.
//   - Cloud compute pricing (reserved-instance / savings-plan bundles):
//     covering exact multi-resource demand (CPU, RAM, GPU) at minimum cost.
//   - Logistics & freight consolidation: selecting mixed-pallet or container
//     rate bundles to ship exact SKU quantities cheaply.
//
// Related LeetCode Problems:
//   - #322 Coin Change [done] (unbounded knapsack minimum-cost DP)
//   - #518 Coin Change II [done] (unbounded combinations counting DP)
//   - #474 Ones and Zeroes [done] (multi-dimensional knapsack DP)
//   - #377 Combination Sum IV [done] (top-down memoized state-space search)
//   - #1301 Number of Paths with Max Score [new] (memoized state transition DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <unordered_map>
#include <vector>

namespace {

// Hash functor for `std::vector<int>` states used as memoization keys.
struct VecHash {
  [[nodiscard]] std::size_t operator()(
      const std::vector<int>& vec) const noexcept {
    std::size_t seed = vec.size();
    for (const int val : vec) {
      // Standard golden-ratio hash combine.
      const std::size_t h = std::hash<int>{}(val);
      seed ^= h + 0x9e3779b9U + (seed << 6U) + (seed >> 2U);
    }
    return seed;
  }
};

class Solution {
 public:
  // Computes the minimum total cost to fulfill `needs` exactly using unit
  // `price`s and repeatable `special` bundle offers.
  // Time: O(S * n * prod(needs[i] + 1)) | Space: O(n * prod(needs[i] + 1)).
  int shoppingOffers(const std::vector<int>& price,
                     const std::vector<std::vector<int>>& special,
                     const std::vector<int>& needs) {
    const std::size_t n = price.size();

    // Pre-filter offers: discard empty bundles (prevents infinite loops) and
    // offers that are not strictly cheaper than buying their items a la carte.
    std::vector<std::vector<int>> useful_offers;
    for (const auto& offer : special) {
      int direct_cost = 0;
      int total_items = 0;
      for (std::size_t i = 0uz; i < n; ++i) {
        direct_cost += (offer[i] * price[i]);
        total_items += offer[i];
      }
      if (total_items > 0 && offer[n] < direct_cost) {
        useful_offers.push_back(offer);
      }
    }

    std::unordered_map<std::vector<int>, int, VecHash> memo;
    return dfs(price, useful_offers, needs, memo);
  }

 private:
  // Computes the baseline cost of buying every remaining item individually.
  static int directPurchaseCost(const std::vector<int>& price,
                                const std::vector<int>& needs) {
    int total = 0;
    for (std::size_t i = 0uz; i < needs.size(); ++i) {
      total += (needs[i] * price[i]);
    }
    return total;
  }

  // Attempts to subtract `offer` from `needs` into `next_needs`. Returns false
  // if applying `offer` would overbuy any item.
  static bool tryApplyOffer(const std::vector<int>& needs,
                            const std::vector<int>& offer,
                            std::vector<int>& next_needs) {
    next_needs.resize(needs.size());
    for (std::size_t i = 0uz; i < needs.size(); ++i) {
      if (offer[i] > needs[i]) {
        return false;
      }
      next_needs[i] = needs[i] - offer[i];
    }
    return true;
  }

  // Memoized DFS returning the minimum cost to satisfy `needs`.
  static int dfs(const std::vector<int>& price,
                 const std::vector<std::vector<int>>& offers,
                 const std::vector<int>& needs,
                 std::unordered_map<std::vector<int>, int, VecHash>& memo) {
    if (const auto it = memo.find(needs); it != memo.end()) {
      return it->second;
    }

    // Baseline: buy all remaining items without any further special offers.
    int best = directPurchaseCost(price, needs);
    const std::size_t n = price.size();
    std::vector<int> next_needs;

    // Try every applicable bundle offer and keep the minimum total cost.
    for (const auto& offer : offers) {
      if (tryApplyOffer(needs, offer, next_needs)) {
        best = std::min(best, offer[n] + dfs(price, offers, next_needs, memo));
      }
    }

    memo.emplace(needs, best);
    return best;
  }
};

LC_PROBLEM(638, "Shopping Offers", "dfs", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<int> price = {2, 5};
    const std::vector<std::vector<int>> special = {{3, 0, 5}, {1, 2, 10}};
    const std::vector<int> needs = {3, 2};
    ctx.expect_eq(sol.shoppingOffers(price, special, needs), 14);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    const std::vector<int> price = {2, 3, 4};
    const std::vector<std::vector<int>> special = {{1, 1, 0, 4}, {2, 2, 1, 9}};
    const std::vector<int> needs = {1, 2, 1};
    ctx.expect_eq(sol.shoppingOffers(price, special, needs), 11);
  });

  ctx.test("python_build_case", [&] {
    Solution sol;
    // Offer [1,2,3] + 1 * item0 ($3) = $6.
    const std::vector<int> price = {3, 4};
    const std::vector<std::vector<int>> special = {{1, 2, 3}, {1, 2, 5}};
    const std::vector<int> needs = {2, 2};
    ctx.expect_eq(sol.shoppingOffers(price, special, needs), 6);
  });

  ctx.test("repeated_offer_application", [&] {
    Solution sol;
    const std::vector<int> price = {5, 5};
    const std::vector<std::vector<int>> special = {{1, 1, 6}};
    const std::vector<int> needs = {3, 3};
    ctx.expect_eq(sol.shoppingOffers(price, special, needs), 18);
  });

  ctx.test("useless_zero_and_overpriced_offers", [&] {
    Solution sol;
    // Zero-item offer [0,0,0] must not infinite-loop; overpriced & overbuying
    // offers are ignored, leaving direct cost 2*2 + 1*3 = 7.
    const std::vector<int> price = {2, 3};
    const std::vector<std::vector<int>> special = {
        {0, 0, 0}, {1, 1, 10}, {3, 0, 1}};
    const std::vector<int> needs = {2, 1};
    ctx.expect_eq(sol.shoppingOffers(price, special, needs), 7);
  });

  ctx.test("zero_needs", [&] {
    Solution sol;
    ctx.expect_eq(sol.shoppingOffers({5, 10}, {{1, 1, 1}}, {0, 0}), 0);
  });
}

}  // namespace
