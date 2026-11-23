// LeetCode 556: Next Greater Element III
// https://leetcode.com/problems/next-greater-element-iii/
//
// Problem:
//   Given a positive 32-bit signed integer `n`, find the smallest positive
//   32-bit signed integer that uses the exact same multiset of decimal digits
//   as `n` and is strictly greater than `n`. Return `-1` if no such integer
//   exists or if the smallest such rearrangement exceeds `INT_MAX` (2^31 - 1).
//
// Examples:
//   Example 1:
//     Input:  n = 230241
//     Output: 230412
//     Explanation:
//       - Convert n to digits = "230241" and scan right-to-left to find the
//         rightmost pivot where digits[i] < digits[i + 1], which is index 3 ('2' <
//         '4').
//       - Reverse the non-increasing suffix "41" starting at index 4 into ascending
//         order "14", producing "230214".
//       - Use upper_bound on the ascending suffix "14" to find the smallest digit
//         strictly greater than '2', which is '4' at index 5, and swap it with
//         digits[3] to get "230412".
//       - Parse "230412" in 64-bit arithmetic and return 230412 since it does not
//         exceed INT_MAX.
//
//   Example 2:
//     Input:  n = 4976
//     Output: 6479
//     Explanation:
//       - Scan digits = "4976" right-to-left (7 >= 6, 9 >= 7, 4 < 9) to identify
//         pivot = 0 with digit '4'.
//       - Reverse the suffix "976" at indices 1..3 into ascending "679", changing
//         digits to "4679".
//       - Find upper_bound('4') in "679", which selects '6' at index 1, and swap it
//         with digits[0] to obtain "6479".
//       - Parse "6479" within 32-bit signed bounds and return 6479.
//
//   Example 3:
//     Input:  n = 2147483486
//     Output: -1
//     Explanation:
//       - Scan digits = "2147483486" right-to-left to locate pivot = 7 where
//         digits[7] = '4' < digits[8] = '8'.
//       - Reverse the suffix "86" into "68" to form "2147483468", then swap
//         digits[7] ('4') with upper_bound result '6' at index 8 to yield
//         "2147483648".
//       - Parse "2147483648" into 2147483648, which exceeds INT_MAX (2147483647),
//         so parse_checked_int32 returns -1.
//
// Algorithm:
//   Convert `n` to its decimal digit sequence and compute its next
//   lexicographical permutation:
//     1. Scan right-to-left to locate the rightmost "pivot" index `pivot`
//        where `digits[pivot] < digits[pivot + 1]`. Because every digit to the
//        right of `pivot` is non-increasing, no permutation of that suffix
//        alone can be larger; if no pivot exists, `n` is already the largest
//        possible arrangement, so return `-1`.
//     2. Reverse the non-increasing suffix `digits[pivot + 1 .. end)` so it
//        becomes sorted ascending.
//     3. Binary-search (`std::ranges::upper_bound`) that sorted suffix for the
//        smallest digit strictly greater than `digits[pivot]` and swap them.
//        Because the suffix was ascending, swapping preserves its sorted order,
//        producing the lexicographically minimal tail.
//     4. Parse the resulting digits into a 64-bit integer and return `-1` if
//        it exceeds `std::numeric_limits<int>::max()`.
//   A companion stack-based variant (`nextGreaterElementStack`) mirrors the
//   same logic by pushing the non-increasing suffix onto a stack (naturally
//   yielding an ascending sequence) and swapping the pivot with the first
//   strictly larger stacked digit.
//   Time Complexity:  O(d), where d = floor(log10(n)) + 1 <= 10 digits.
//   Space Complexity: O(d) for storing the decimal digits.
//
// Data Structures:
//   - Decimal digit buffer (`std::string`): supports in-place suffix reversal
//     and binary search.
//   - Ascending digit stack (`std::vector<char>` in the stack variant): stores
//     the traversed suffix in ascending order for pivot replacement.
//
// Real-World Applications:
//   - Combinatorial enumeration and lexicographical ranking/unranking in test
//     case generators and constraint solvers.
//   - Cryptographic key-space / passcode traversal requiring deterministic
//     successors with fixed character multisets.
//   - Sequence optimization in scheduling where tasks are rearranged to the
//     next lexicographical ordering.
//
// Related LeetCode Problems:
//   - #31 Next Permutation [done] (general next lexicographical permutation)
//   - #496 Next Greater Element I [done] (monotonic stack next greater value)
//   - #503 Next Greater Element II [done] (circular next greater element)
//   - #1850 Minimum Adjacent Swaps to Reach the Kth Smallest Number [new] (repeated next permutation)
//   - #1842 Next Palindrome Using Same Digits [new] (next permutation on half-string)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Computes the smallest 32-bit integer strictly greater than `n` with the
  // same decimal digits via suffix reversal + binary search.
  // Time: O(d), Space: O(d), where d <= 10 is the number of decimal digits.
  int nextGreaterElement(int n) {
    if (n < 10) {
      return -1;
    }

    std::string digits = std::to_string(n);

    // Phase 1: Find the rightmost index whose digit is smaller than its right
    // neighbor. Everything to its right is non-increasing.
    std::size_t pivot = digits.size();
    for (std::size_t i = digits.size() - 1uz; i-- > 0uz;) {
      if (digits[i] < digits[i + 1uz]) {
        pivot = i;
        break;
      }
    }
    if (pivot == digits.size()) {
      return -1;
    }

    // Phase 2: Reverse the non-increasing suffix so it becomes sorted ascending.
    const auto suffix_begin =
        digits.begin() + static_cast<std::ptrdiff_t>(pivot + 1uz);
    std::ranges::reverse(suffix_begin, digits.end());

    // Phase 3: Swap the pivot with the smallest digit strictly greater than it
    // in the ascending suffix; the suffix stays sorted afterward.
    auto swap_it =
        std::ranges::upper_bound(suffix_begin, digits.end(), digits[pivot]);
    std::swap(digits[pivot], *swap_it);

    return parse_checked_int32(digits);
  }

  // Companion stack-based approach: collects the non-increasing suffix onto a
  // stack (producing ascending order) until a smaller pivot digit is found.
  // Time: O(d), Space: O(d).
  int nextGreaterElementStack(int n) {
    if (n < 10) {
      return -1;
    }

    std::string digits = std::to_string(n);
    std::vector<char> stack;
    stack.reserve(digits.size());

    for (std::size_t i = digits.size() - 1uz; i > 0uz; --i) {
      stack.push_back(digits[i]);
      const char pivot = digits[i - 1uz];
      if (pivot >= digits[i]) {
        continue;
      }

      // `stack` is already sorted ascending because the traversed suffix was
      // non-increasing right-to-left. Swap `pivot` with the first larger digit.
      for (char& candidate : stack) {
        if (candidate > pivot) {
          digits[i - 1uz] = candidate;
          candidate = pivot;
          break;
        }
      }

      // Write back the still-ascending tail after the updated pivot position.
      for (std::size_t k = 0uz; k < stack.size(); ++k) {
        digits[i + k] = stack[k];
      }
      return parse_checked_int32(digits);
    }

    return -1;
  }

 private:
  static constexpr std::int64_t kBase = 10;

  // Converts decimal digits to a 32-bit signed integer, returning `-1` on
  // 32-bit signed overflow.
  static int parse_checked_int32(const std::string& digits) {
    std::int64_t value = 0;
    for (char ch : digits) {
      value = (value * kBase) + static_cast<std::int64_t>(ch - '0');
    }
    if (value > std::numeric_limits<int>::max()) {
      return -1;
    }
    return static_cast<int>(value);
  }
};

LC_PROBLEM(556, "Next Greater Element III", "stack", ctx) {
  auto verify_both = [&](int input, int expected) {
    Solution sol;
    ctx.expect_eq(sol.nextGreaterElement(input), expected);
    ctx.expect_eq(sol.nextGreaterElementStack(input), expected);
  };

  ctx.test("basic_examples", [&] {
    verify_both(12, 21);
    verify_both(21, -1);
  });

  ctx.test("source_build_case", [&] { verify_both(4976, 6479); });

  ctx.test("duplicate_digits_and_zero", [&] {
    verify_both(230241, 230412);
    verify_both(101, 110);
    verify_both(111, -1);
  });

  ctx.test("single_digit_and_descending", [&] {
    verify_both(1, -1);
    verify_both(9, -1);
    verify_both(987654321, -1);
  });

  ctx.test("int32_overflow_boundary", [&] {
    // Next permutation is 2147483647 == INT_MAX (valid).
    verify_both(2147483476, 2147483647);
    // Next permutation is 2147483648 == INT_MAX + 1 (overflow -> -1).
    verify_both(2147483486, -1);
    // Next permutation is 9199999999 (overflow -> -1).
    verify_both(1999999999, -1);
  });
}

}  // namespace
