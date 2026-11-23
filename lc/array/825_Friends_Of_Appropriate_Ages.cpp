// LeetCode 825: Friends Of Appropriate Ages
// https://leetcode.com/problems/friends-of-appropriate-ages/
//
// Problem:
//   Given an array `ages` where `ages[i]` is the age of the i-th person, count
//   the total number of directed friend requests made. Person `x` will NOT
//   send a friend request to person `y` (`x != y`) if any of the following
//   holds:
//     1. `ages[y] <= 0.5 * ages[x] + 7`
//     2. `ages[y] > ages[x]`
//     3. `ages[y] > 100 && ages[x] < 100`
//   Otherwise, `x` sends a friend request to `y`. People never send requests
//   to themselves.
//   Constraints: 1 <= ages.size() <= 20,000; 1 <= ages[i] <= 120.
//
// Algorithm:
//   Condition 3 is redundant because `ages[y] > 100 && ages[x] < 100` already
//   implies `ages[y] > ages[x]` (Condition 2). Thus person `x` sends a request
//   to person `y` if and only if `(ages[x] / 2) + 7 < ages[y] <= ages[x]`.
//   Note that for integer `a <= 14`, `(a / 2) + 7 >= a`, so no valid target
//   age exists. Because ages are bounded in `[1, 120]`, we count the frequency
//   of each age in a fixed array `counts[121]` and compute its prefix sum
//   `prefix[121]`. For each age `a` in `[15, 120]` with `counts[a] > 0`, the
//   number of people whose age falls in `((a / 2) + 7, a]` is
//   `in_range = prefix[a] - prefix[(a / 2) + 7]`. Each of the `counts[a]`
//   people sends requests to everyone in that range except themselves, adding
//   `counts[a] * (in_range - 1)`.
//   We also include the frequency-map pairwise method (`numFriendRequestsPairwise`)
//   for reference and cross-verification in tests.
//   Time Complexity:  O(n + M), where M = 120 is the maximum possible age.
//   Space Complexity: O(M) = O(1) fixed-size arrays.
//
// Data Structures:
//   - Counting Array (`std::array<int, 121>`): aggregates duplicate ages into
//     O(1)-indexed buckets.
//   - Prefix Sum Array (`std::array<int, 121>`): answers range-count queries
//     over `((a / 2) + 7, a]` in O(1) per age bucket.
//
// Real-World Applications:
//   - Social graph & recommendation filtering: fast range-eligibility counting
//     and candidate sizing over bounded demographic buckets (age, tier, rank).
//   - Matchmaking systems: estimating compatible player pools within asymmetric
//     skill-rating bands using histogram prefix sums.
//   - Database query cardinality estimation: answering inequality-join counts
//     over low-cardinality integer columns via cumulative histograms.
//
// Related LeetCode Problems:
//   - #167 Two Sum II [done] - Input Array Is Sorted (bounded range matching on ordered values)
//   - #274 H-Index [done] (counting sort / bucket histogram over a bounded domain)
//   - #1099 Two Sum Less Than K [done] (counting pairs satisfying linear inequalities)
//   - #2563 Count the Number of Fair Pairs [new] (counting pairs within lower and upper bounds)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

constexpr std::size_t kMaxAge = 120uz;
// For age <= 14, (age / 2) + 7 >= age, so no target age satisfies the strict
// lower bound (age / 2) + 7 < target <= age.
constexpr std::size_t kMinRequestingAge = 15uz;

class Solution {
 public:
  // Counts total friend requests in O(n + kMaxAge) using bucketed prefix sums.
  // Time: O(n + M), Space: O(M) where M = 120.
  int numFriendRequests(const std::vector<int>& ages) {
    std::array<int, kMaxAge + 1uz> counts{};
    for (const int age : ages) {
      if (age >= 1 && std::cmp_less_equal(age, kMaxAge)) {
        ++counts[static_cast<std::size_t>(age)];
      }
    }

    // `prefix[a]` holds the total number of people with age <= a.
    std::array<int, kMaxAge + 1uz> prefix{};
    for (std::size_t age = 1uz; age <= kMaxAge; ++age) {
      prefix[age] = prefix[age - 1] + counts[age];
    }

    int total_requests = 0;
    for (std::size_t age = kMinRequestingAge; age <= kMaxAge; ++age) {
      if (counts[age] == 0) {
        continue;
      }

      // Valid recipient ages lie strictly above `(age / 2) + 7` and at most
      // `age`. Because `age >= 15`, `lower_exclusive < age`, so `age` itself
      // is always included in `in_range` and `in_range >= counts[age] >= 1`.
      const std::size_t lower_exclusive = (age / 2uz) + 7uz;
      const int in_range = prefix[age] - prefix[lower_exclusive];
      // Subtract 1 from `in_range` to exclude self-requests.
      total_requests += counts[age] * (in_range - 1);
    }

    return total_requests;
  }

  // Alternative frequency-map pairwise approach mirroring the direct rule
  // check over distinct age pairs.
  // Time: O(n + U^2), Space: O(U) where U <= 120 is the count of unique ages.
  int numFriendRequestsPairwise(const std::vector<int>& ages) {
    std::unordered_map<int, int> freq;
    for (const int age : ages) {
      ++freq[age];
    }

    int total_requests = 0;
    for (const auto& [age_a, count_a] : freq) {
      for (const auto& [age_b, count_b] : freq) {
        if (canRequest(age_a, age_b)) {
          if (age_a == age_b) {
            total_requests += count_a * (count_b - 1);
          } else {
            total_requests += count_a * count_b;
          }
        }
      }
    }
    return total_requests;
  }

 private:
  // Evaluates the problem's exact eligibility predicate using integer math:
  // `age_b <= 0.5 * age_a + 7` is equivalent to `2 * age_b <= age_a + 14`.
  static bool canRequest(int age_a, int age_b) {
    if ((2 * age_b) <= (age_a + 14)) {
      return false;
    }
    if (age_b > age_a) {
      return false;
    }
    return age_b <= 100 || age_a >= 100;
  }
};

LC_PROBLEM(825, "Friends Of Appropriate Ages", "array", ctx) {
  Solution sol;

  auto expect_both = [&](const std::vector<int>& ages, int expected) {
    ctx.expect_eq(sol.numFriendRequests(ages), expected);
    ctx.expect_eq(sol.numFriendRequestsPairwise(ages), expected);
  };

  ctx.test("example_1_same_age_pair", [&] { expect_both({16, 16}, 2); });

  ctx.test("example_2_consecutive_ages", [&] { expect_both({16, 17, 18}, 2); });

  ctx.test("example_3_mixed_and_over_100",
           [&] { expect_both({20, 30, 100, 110, 120}, 3); });

  ctx.test("python_build_additional_cases", [&] {
    expect_both({15, 15, 15, 16, 16, 16}, 12);
    expect_both({17, 18}, 1);
    expect_both({68, 13, 18, 20, 105}, 2);
  });

  ctx.test("all_too_young_no_requests", [&] {
    expect_both({1, 10, 14, 14, 14}, 0);
  });

  ctx.test("single_person_never_self_requests", [&] {
    expect_both({25}, 0);
  });
}

}  // namespace
