// LeetCode 443: String Compression
// https://leetcode.com/problems/string-compression/
//
// Problem:
//   Given an array of characters `chars`, compress it in-place using run-length
//   encoding: for each maximal group of consecutive identical characters, write
//   the character once, followed by the decimal digits of the group's length if
//   the length is strictly greater than 1 (each digit stored as a separate char
//   element). Return the new compressed length using only O(1) extra space.
//
// Examples:
//   Example 1:
//     Input:  chars = ['a', 'a', 'b', 'b', 'c', 'c', 'c']
//     Output: 6 (chars[0..5] = ['a', '2', 'b', '2', 'c', '3'])
//     Explanation:
//       - First run 'a' spans indices 0..1 (run_len = 2), writing 'a' and '2' at
//         chars[0..1] and advancing to read = 2, write = 2.
//       - Second run 'b' spans indices 2..3 (run_len = 2), writing 'b' and '2' at
//         chars[2..3] and advancing to read = 4, write = 4.
//       - Third run 'c' spans indices 4..6 (run_len = 3), writing 'c' and '3' at
//         chars[4..5] and advancing to read = 7, write = 6.
//       - Return write = 6 with compressed prefix ['a', '2', 'b', '2', 'c', '3'].
//
//   Example 2:
//     Input:  chars = ['a', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
//             'b']
//     Output: 4 (chars[0..3] = ['a', 'b', '1', '2'])
//     Explanation:
//       - First run 'a' at index 0 has run_len = 1, so only 'a' is written at
//         chars[0] and read = 1, write = 1.
//       - Second run 'b' spans indices 1..12 (run_len = 12), writing 'b' at
//         chars[1] and setting digits_start = 2, write = 2, read = 13.
//       - Emitting digits of 12 via remaining % 10 writes '2' then '1' at
//         chars[2..3] (write = 4), and std::ranges::reverse flips chars[2..4] to
//         '1', '2'.
//       - Return write = 4 with compressed prefix ['a', 'b', '1', '2'].
//
//   Example 3:
//     Input:  chars = ['a', 'b', 'c', 'd']
//     Output: 4 (chars[0..3] = ['a', 'b', 'c', 'd'])
//     Explanation:
//       - Start with read = 0 and write = 0; each character forms a maximal run of
//         length run_len = 1.
//       - For each run, chars[write++] = ch writes the character and skips the `if
//         (run_len > 1)` digit-emission block.
//       - When read reaches 4, return write = 4 with chars unchanged as ['a', 'b',
//         'c', 'd'].
//
// Algorithm:
//   Two-pointer in-place run-length encoding with `read` and `write` indices:
//     - Outer loop scans one maximal run of equal characters starting at
//       `group_start = read`, advancing `read` until the character changes or
//       the array ends.
//     - Write the run's character at `chars[write++]`.
//     - If `run_len = read - group_start` is greater than 1, emit its decimal
//       digits in-place into `chars[write++]` from least-significant to most-
//       significant, then reverse that digit slice in-place.
//   Safety invariant: for every run of length L >= 1, the compressed output
//   uses 1 char (L == 1) or 1 + floor(log10(L)) + 1 <= L chars (L >= 2), so
//   `write <= read` holds at every point and unread input is never overwritten.
//   Time complexity: O(n) — each character is read once and written at most
//   twice (digit emit + reverse).
//   Space complexity: O(1) — zero heap allocations; digits are formatted
//   directly in the destination buffer.
//
// Data Structures:
//   - In-place std::vector<char> (`chars`): serves simultaneously as input
//     stream and output buffer.
//   - Two index pointers (`read`, `write`) and a std::span view for reversing
//     emitted multi-digit counts in-place.
//
// Real-World Applications:
//   - Run-length encoding (RLE) stages in image formats (BMP, TIFF, PCX, PackBits)
//     and fax/bitmap compression.
//   - Columnar analytics storage engines (Parquet, ORC, ClickHouse) compressing
//     sorted low-cardinality columns with RLE.
//   - Telemetry and sparse time-series encoding where repeated sensor readings
//     are collapsed into (value, count) pairs.
//
// Related LeetCode Problems:
//   - #38 Count and Say [done] (iterative run-length encoding of digit strings)
//   - #271 Encode and Decode Strings [done] (length-prefixed string serialization)
//   - #408 Valid Word Abbreviation [done] (validating numeric run-length skips)
//   - #604 Design Compressed String Iterator [new] (streaming decoder for RLE strings)
//   - #1531 String Compression II [done] (DP minimizing RLE length with up to k deletions)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <span>
#include <vector>

namespace {

class Solution {
 public:
  // Compresses `chars` in-place via run-length encoding and returns the new
  // logical length of the compressed prefix.
  //
  // Invariant: `write <= read` throughout the pass because every group of
  // length L compresses to at most L characters (1 if L == 1; 1 + digits(L) <=
  // L if L >= 2), guaranteeing unread characters are never overwritten.
  //
  // Time: O(n), Space: O(1).
  int compress(std::vector<char>& chars) {
    std::size_t read = 0;
    std::size_t write = 0;

    while (read < chars.size()) {
      const char ch = chars[read];
      const std::size_t group_start = read;

      // Consume the entire maximal contiguous run of `ch`.
      while (read < chars.size() && chars[read] == ch) {
        ++read;
      }

      const std::size_t run_len = read - group_start;
      chars[write] = ch;
      ++write;

      // Only append the count when the character repeats (run_len > 1).
      if (run_len > 1) {
        // Emit decimal digits in reverse directly into `chars`, then reverse
        // the slice in-place for strict O(1) extra space.
        const std::size_t digits_start = write;
        std::size_t remaining = run_len;
        while (remaining > 0) {
          chars[write] = static_cast<char>('0' + (remaining % 10));
          ++write;
          remaining /= 10;
        }
        std::ranges::reverse(
            std::span(chars).subspan(digits_start, write - digits_start));
      }
    }

    return static_cast<int>(write);
  }
};

LC_PROBLEM(443, "String Compression", "two_pointers", ctx) {
  Solution sol;

  auto prefix = [](const std::vector<char>& chars, int len) {
    return std::vector<char>(chars.begin(),
                             chars.begin() + static_cast<std::ptrdiff_t>(len));
  };

  ctx.test("example_1_standard", [&] {
    std::vector<char> chars{'a', 'a', 'b', 'b', 'c', 'c', 'c'};
    const int len = sol.compress(chars);
    ctx.expect_eq(len, 6);
    ctx.expect_eq(prefix(chars, len),
                  std::vector<char>{'a', '2', 'b', '2', 'c', '3'});
  });

  ctx.test("docstring_variant", [&] {
    std::vector<char> chars{'a', 'a', 'b', 'b', 'b', 'c', 'c', 'c'};
    const int len = sol.compress(chars);
    ctx.expect_eq(len, 6);
    ctx.expect_eq(prefix(chars, len),
                  std::vector<char>{'a', '2', 'b', '3', 'c', '3'});
  });

  ctx.test("example_2_single_char", [&] {
    std::vector<char> chars{'a'};
    const int len = sol.compress(chars);
    ctx.expect_eq(len, 1);
    ctx.expect_eq(prefix(chars, len), std::vector<char>{'a'});
  });

  ctx.test("example_3_multi_digit_run", [&] {
    std::vector<char> chars{'a', 'b', 'b', 'b', 'b', 'b', 'b',
                            'b', 'b', 'b', 'b', 'b', 'b'};
    const int len = sol.compress(chars);
    ctx.expect_eq(len, 4);
    ctx.expect_eq(prefix(chars, len), std::vector<char>{'a', 'b', '1', '2'});
  });

  ctx.test("python_build_case", [&] {
    std::vector<char> chars{'a', 'a', 'a', 'a', 'a', 'a', 'a',
                            'a', 'a', 'a', 'b', 'b', 'b', 'c'};
    const int len = sol.compress(chars);
    ctx.expect_eq(len, 6);
    ctx.expect_eq(prefix(chars, len),
                  std::vector<char>{'a', '1', '0', 'b', '3', 'c'});
  });

  ctx.test("empty_and_all_unique", [&] {
    std::vector<char> empty{};
    ctx.expect_eq(sol.compress(empty), 0);

    std::vector<char> unique{'a', 'b', 'c', 'd'};
    const int len = sol.compress(unique);
    ctx.expect_eq(len, 4);
    ctx.expect_eq(prefix(unique, len), std::vector<char>{'a', 'b', 'c', 'd'});
  });

  ctx.test("three_digit_run", [&] {
    std::vector<char> chars(100, 'z');
    const int len = sol.compress(chars);
    ctx.expect_eq(len, 4);
    ctx.expect_eq(prefix(chars, len), std::vector<char>{'z', '1', '0', '0'});
  });
}

}  // namespace
