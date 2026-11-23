// LeetCode 465: Optimal Account Balancing
// https://leetcode.com/problems/optimal-account-balancing/
//
// Problem:
//   You are given an array of transactions where `transactions[i] =
//   [from_i, to_i, amount_i]` means person `from_i` gave `amount_i` dollars
//   to person `to_i`. Person IDs need not be contiguous. Return the minimum
//   number of money transfers required to settle all debts so that every
//   person's net balance becomes zero.
//
// Examples:
//   Example 1:
//     Input:  transactions = [[0, 1, 10], [2, 0, 5]]
//     Output: 2
//     Explanation:
//       - Aggregate net balances into balances[0] = -5, balances[1] = +10, and
//         balances[2] = -5, producing three non-zero entries in debts.
//       - In dfsSettle, settling a -5 balance against opposite-sign +10 transfers
//         -5 onto +10 (leaving +5) in 1 transfer.
//       - The remaining +5 and -5 balances sum to 0, canceling in 1 final transfer
//         (and breaking early via exact-match pruning) for a minimum of 2
//         transfers.
//
//   Example 2:
//     Input:  transactions = [[0, 1, 10], [1, 0, 1], [1, 2, 5], [2, 0, 5]]
//     Output: 1
//     Explanation:
//       - Compute net balances: person 0 has -10 + 1 + 5 = -4, person 1 has +10 - 1
//         - 5 = +4, and person 2 has +5 - 5 = 0.
//       - Discard person 2's zero balance so debts contains only -4 and +4.
//       - dfsSettle pairs -4 with +4 in a single transfer (-4 + 4 == 0) and returns
//         1.
//
//   Example 3:
//     Input:  transactions = [[1, 0, 3], [2, 0, 7], [4, 3, 5]]
//     Output: 3
//     Explanation:
//       - Aggregate net balances: person 0 has +10, person 1 has -3, person 2 has
//         -7, person 3 has +5, and person 4 has -5 (five non-zero entries).
//       - The +5 and -5 pair cancels exactly in 1 transfer, triggering the
//         exact-cancellation break so no suboptimal splits are explored for that
//         account.
//       - The remaining three-person zero-sum group {+10, -3, -7} settles in 2
//         transfers (e.g., transferring -3 onto +10 leaves +7 to cancel with -7),
//         totaling 1 + 2 = 3 transfers.
//
// Algorithm:
//   First aggregate every person's net balance across all transactions
//   (`balance[from] -= amount`, `balance[to] += amount`) and discard anyone
//   whose net balance is already 0, leaving `M` people with non-zero debts.
//   Then run backtracking DFS over the non-zero balances: at the first
//   unsettled index `start`, pair `debts[start]` with every later index `i`
//   that has the opposite sign (creditor vs. debtor), transfer `debts[start]`
//   onto `debts[i]` to zero out `start`, recurse on `start + 1`, and restore
//   `debts[i]` afterward. Two provably optimal prunings accelerate search:
//   (1) if `debts[i] + debts[start] == 0`, an exact two-person cancellation is
//   always part of an optimal partition into zero-sum subsets, so we stop
//   trying further partners for `start`; (2) skip duplicate debt values at the
//   same recursion depth since they produce symmetric subproblems.
//   Time Complexity:  O((M - 1)!) worst case, where M is the number of people
//                     with non-zero net balance (M <= 12 in constraints),
//                     heavily pruned in practice.
//   Space Complexity: O(M) for the net-balance array and recursion stack.
//
// Data Structures:
//   - Hash map (`std::unordered_map<int, int>`): accumulates net balances for
//     arbitrary non-contiguous person IDs.
//   - Dense vector (`std::vector<int>`): stores only non-zero balances mutated
//     in-place during DFS backtracking.
//   - Per-level hash set (`std::unordered_set<int>`): deduplicates identical
//     counterparty balances within a single branching level.
//
// Real-World Applications:
//   - Expense-splitting apps (Splitwise, Tricount): minimizing the number of
//     group reimbursements after shared trips or household bills.
//   - Interbank & clearing-house netting: collapsing multilateral payment
//     obligations into the fewest possible settlement wire transfers.
//   - Supply-chain inventory rebalancing: minimizing cross-warehouse shipments
//     needed to reconcile surplus and deficit stock levels.
//
// Related LeetCode Problems:
//   - #698 Partition to K Equal Sum Subsets [done] (backtracking subset partition)
//   - #473 Matchsticks to Square [new] (DFS partitioning with duplicate pruning)
//   - #1986 Minimum Number of Work Sessions to Finish the Tasks [new] (subset DP/DFS)
//   - #2305 Fair Distribution of Cookies [new] (small-N backtracking optimization)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the minimum number of transfers required to settle all debts.
  // Time: O((M - 1)!) worst case (M = non-zero accounts) | Space: O(M).
  int minTransfers(const std::vector<std::vector<int>>& transactions) {
    // Aggregate net balance per person: outgoing lowers balance, incoming raises.
    std::unordered_map<int, int> balances;
    for (const auto& tx : transactions) {
      balances[tx[0]] -= tx[2];
      balances[tx[1]] += tx[2];
    }

    // Only accounts with a non-zero net balance participate in settlement.
    std::vector<int> debts;
    debts.reserve(balances.size());
    for (const auto& entry : balances) {
      if (entry.second != 0) {
        debts.push_back(entry.second);
      }
    }

    if (debts.empty()) {
      return 0;
    }

    return dfsSettle(debts, 0uz);
  }

 private:
  // Backtracking DFS that zeroes out `debts[start]` by transferring its balance
  // to an opposite-sign account `debts[i]` (i > start) and returns the minimum
  // number of transactions needed to settle `debts[start..]`.
  static int dfsSettle(std::vector<int>& debts, std::size_t start) {
    // Advance past accounts that were already settled by earlier transfers.
    while (start < debts.size() && debts[start] == 0) {
      ++start;
    }
    if (start == debts.size()) {
      return 0;
    }

    // Upper bound: settling `k` non-zero accounts never exceeds `k - 1` transfers,
    // so `debts.size()` is strictly greater than any valid answer.
    int min_tx = static_cast<int>(debts.size());
    std::unordered_set<int> tried_values;

    for (std::size_t i = start + 1uz; i < debts.size(); ++i) {
      // Only pair opposite signs (creditor with debtor); avoid signed multiply
      // overflow by comparing signs directly.
      if ((debts[i] < 0) == (debts[start] < 0) || debts[i] == 0) {
        continue;
      }

      // Skip equivalent counterparty amounts at this recursion depth.
      if (!tried_values.insert(debts[i]).second) {
        continue;
      }

      // Transfer `debts[start]` onto `debts[i]` so `start` becomes settled.
      const int orig_partner = debts[i];
      debts[i] += debts[start];
      min_tx = std::min(min_tx, 1 + dfsSettle(debts, start + 1uz));
      debts[i] = orig_partner;

      // Optimal pruning: an exact opposite match forms a minimal 2-person
      // zero-sum subset, which is always optimal to take immediately.
      if (orig_partner + debts[start] == 0) {
        break;
      }
    }

    return min_tx;
  }
};

LC_PROBLEM(465, "Optimal Account Balancing", "dfs", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::vector<int>> tx = {{0, 1, 10}, {2, 0, 5}};
    ctx.expect_eq(sol.minTransfers(tx), 2);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    const std::vector<std::vector<int>> tx = {
        {0, 1, 10}, {1, 0, 1}, {1, 2, 5}, {2, 0, 5}};
    ctx.expect_eq(sol.minTransfers(tx), 1);
  });

  ctx.test("python_build_non_contiguous_ids", [&] {
    Solution sol;
    // Net balances: 1:-8, 5:+8, 8:-8, 9:+8, 2:-9, 3:+10, 4:-1 -> 3 subsets, 4 tx.
    const std::vector<std::vector<int>> tx = {
        {1, 5, 8}, {8, 9, 8}, {2, 3, 9}, {4, 3, 1}};
    ctx.expect_eq(sol.minTransfers(tx), 4);
  });

  ctx.test("already_balanced_and_empty", [&] {
    Solution sol;
    ctx.expect_eq(sol.minTransfers({}), 0);
    const std::vector<std::vector<int>> cycle = {
        {0, 1, 5}, {1, 2, 5}, {2, 0, 5}};
    ctx.expect_eq(sol.minTransfers(cycle), 0);
  });

  ctx.test("exact_cancellation_vs_group_split", [&] {
    Solution sol;
    // Net balances: 0:+10, 1:-3, 2:-7, 3:+5, 4:-5 -> {+10,-3,-7} & {+5,-5} -> 3 tx.
    const std::vector<std::vector<int>> tx = {
        {1, 0, 3}, {2, 0, 7}, {4, 3, 5}};
    ctx.expect_eq(sol.minTransfers(tx), 3);
  });
}

}  // namespace
