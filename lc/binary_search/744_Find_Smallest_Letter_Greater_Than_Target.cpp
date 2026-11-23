// LeetCode 744: Find Smallest Letter Greater Than Target
// https://leetcode.com/problems/find-smallest-letter-greater-than-target/
//
// Problem:
//   Given a non-decreasing array of lowercase English characters `letters` and
//   a character `target`, return the smallest character in `letters` that is
//   lexicographically strictly greater than `target`. If no such character
//   exists, wrap around and return `letters[0]`.
//   Constraints: 2 <= letters.length <= 10^4, `letters` contains at least 2
//   unique lowercase letters and is sorted in non-decreasing order.
//
// Algorithm:
//   Perform an upper-bound binary search over the half-open index range
//   `[0, n)` to find the first position `lhs` where `letters[lhs] > target`:
//     - If `letters[mid] > target`, `mid` is a valid candidate, so shrink to
//       `[lhs, mid)`.
//     - Otherwise `letters[mid] <= target`, so discard `mid` and shrink to
//       `[mid + 1, rhs)`.
//   When the loop terminates (`lhs == rhs`), `lhs == n` means every character
//   is `<= target`, so wrap around to `letters[0]`; otherwise return
//   `letters[lhs]`.
//   Time Complexity: O(log n).
//   Space Complexity: O(1).
//
// Data Structures:
//   - Sorted `std::vector<char>` searched in-place using two `std::size_t`
//     boundary indices.
//
// Real-World Applications:
//   - Consistent hashing rings (finding the next clockwise server node after a
//     hashed key, wrapping around to the first node on the ring).
//   - Circular event schedulers and cron timers selecting the next upcoming
//     trigger slot in a sorted repeating schedule.
//   - Autocomplete / dictionary prefix navigation advancing past a current key.
//
// Related LeetCode Problems:
//   - #34 Find First and Last Position of Element in Sorted Array [done] (lower- and upper-bound binary search)
//   - #35 Search Insert Position [done] (lower-bound binary search on sorted array)
//   - #704 Binary Search [done] (exact target lookup via binary search)
//   - #278 First Bad Version [done] (finding first index satisfying a monotonic predicate)

#include "framework/lc_framework.h"

#include <cstddef>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the smallest letter strictly greater than `target`, wrapping around
  // to `letters[0]` if none exists.
  // Time: O(log n), Space: O(1).
  char nextGreatestLetter(const std::vector<char>& letters, char target) {
    // Half-open upper-bound search over [lhs, rhs).
    // Invariant: for all i < lhs, letters[i] <= target;
    //            for all i >= rhs, letters[i] > target.
    std::size_t lhs = 0uz;
    std::size_t rhs = letters.size();

    while (lhs < rhs) {
      const std::size_t mid = lhs + ((rhs - lhs) / 2uz);
      if (letters[mid] > target) {
        rhs = mid;
      } else {
        lhs = mid + 1uz;
      }
    }

    return lhs == letters.size() ? letters[0] : letters[lhs];
  }
};

LC_PROBLEM(744, "Find Smallest Letter Greater Than Target", "binary_search",
           ctx) {
  ctx.test("docstring_examples", [&] {
    Solution sol;
    const std::vector<char> letters = {'c', 'f', 'j'};
    ctx.expect_eq(sol.nextGreatestLetter(letters, 'a'), 'c');
    ctx.expect_eq(sol.nextGreatestLetter(letters, 'c'), 'f');
    ctx.expect_eq(sol.nextGreatestLetter(letters, 'd'), 'f');
    ctx.expect_eq(sol.nextGreatestLetter(letters, 'g'), 'j');
    ctx.expect_eq(sol.nextGreatestLetter(letters, 'j'), 'c');
    ctx.expect_eq(sol.nextGreatestLetter(letters, 'k'), 'c');
  });

  ctx.test("wrap_around_at_z", [&] {
    Solution sol;
    ctx.expect_eq(sol.nextGreatestLetter({'a', 'b'}, 'z'), 'a');
  });

  ctx.test("duplicate_letters", [&] {
    Solution sol;
    const std::vector<char> letters = {'c', 'c', 'f', 'f', 'j', 'j'};
    ctx.expect_eq(sol.nextGreatestLetter(letters, 'c'), 'f');
    ctx.expect_eq(sol.nextGreatestLetter(letters, 'f'), 'j');
    ctx.expect_eq(sol.nextGreatestLetter(letters, 'j'), 'c');
  });
}

}  // namespace
