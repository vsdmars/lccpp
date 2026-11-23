// LeetCode 1672: Richest Customer Wealth
// https://leetcode.com/problems/richest-customer-wealth/
//
// Problem:
//   Given an `m x n` integer grid `accounts` where `accounts[i][j]` is the
//   balance that the i-th customer holds in the j-th bank, return the wealth
//   of the richest customer. A customer's wealth is the sum of their balances
//   across all `n` banks.
//   Constraints: 1 <= m, n <= 50; 1 <= accounts[i][j] <= 100.
//
// Algorithm:
//   Row-wise Reduction + Running Maximum:
//   Iterate over every customer row in `accounts`, sum the balances in that
//   row using `std::accumulate`, and maintain the maximum row sum observed so
//   far. Because each grid cell is visited exactly once and no intermediate
//   array of row sums is allocated, the pass is optimal in both time and space.
//   Time Complexity:  O(m * n)
//   Space Complexity: O(1)
//
// Data Structures:
//   - Read-only 2D matrix (`std::vector<std::vector<int>>`) scanned row by row
//     with a single scalar accumulator for the running maximum wealth.
//
// Real-World Applications:
//   - Financial reporting and portfolio aggregation (finding peak customer
//     exposure across multi-account ledgers).
//   - Resource telemetry and cloud billing (identifying the highest-spend
//     tenant across regional service usage tables).
//   - Map-reduce / SQL `MAX(SUM(...))` group-by aggregations in analytics
//     engines.
//
// Related LeetCode Problems:
//   - #1480 Running Sum of 1d Array [done] (fundamental prefix-sum accumulation on arrays)
//   - #1572 Matrix Diagonal Sum [new] (2D grid element summation)
//   - #2643 Row With Maximum Ones [new] (finding the row maximizing a row-level aggregate)
//   - #53 Maximum Subarray [done] (maximizing a sum over array slices)

#include "framework/lc_framework.h"

#include <algorithm>
#include <numeric>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the maximum total wealth across all customers.
  //
  // Invariant: `max_wealth` holds the highest row sum among all customer rows
  // processed so far (or 0 if `accounts` is empty).
  //
  // Time: O(m * n), Space: O(1).
  int maximumWealth(const std::vector<std::vector<int>>& accounts) {
    int max_wealth = 0;

    for (const auto& customer_banks : accounts) {
      // Aggregate all bank balances for the current customer.
      const int wealth =
          std::accumulate(customer_banks.begin(), customer_banks.end(), 0);
      max_wealth = std::max(max_wealth, wealth);
    }

    return max_wealth;
  }
};

LC_PROBLEM(1672, "Richest Customer Wealth", "array", ctx) {
  ctx.test("example_1_tied_richest", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumWealth({{1, 2, 3}, {3, 2, 1}}), 6);
  });

  ctx.test("example_2_middle_customer_richest", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumWealth({{1, 5}, {7, 3}, {3, 5}}), 10);
  });

  ctx.test("example_3_third_customer_richest", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumWealth({{2, 8, 7}, {7, 1, 3}, {1, 9, 5}}), 17);
  });

  ctx.test("single_customer_single_bank", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumWealth({{42}}), 42);
  });

  ctx.test("single_bank_multiple_customers", [&] {
    Solution sol;
    ctx.expect_eq(sol.maximumWealth({{5}, {99}, {12}}), 99);
  });
}

}  // namespace
