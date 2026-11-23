// LeetCode 6: Zigzag Conversion
// https://leetcode.com/problems/zigzag-conversion/
//
// Problem:
//   Given a string `s` and a positive integer `numRows`, write the characters
//   of `s` in a downward-then-diagonally-upward zigzag pattern across `numRows`
//   rows, and return the string formed by reading all rows sequentially from
//   top to bottom.
//
//   Constraints:
//   - 1 <= s.length <= 1000
//   - s consists of English letters (lower-case and upper-case), ',' and '.'
//   - 1 <= numRows <= 1000
//
// Algorithm:
//   Row-by-row simulation using a direction toggle. If `numRows <= 1` or
//   `s.size() <= numRows`, no zigzag folding occurs and `s` is returned as-is.
//   Otherwise, allocate `numRows` string buffers and iterate over `s` once,
//   appending each character to `rows[row]`. Whenever `row` reaches the top
//   boundary (`0`) or bottom boundary (`numRows - 1`), flip the vertical
//   direction (`going_down`). Finally, concatenate all row buffers in order
//   into a pre-reserved output string.
//
//   Time Complexity:  O(n), where n = s.size(), since each character is
//                     appended once and copied once during concatenation.
//   Space Complexity: O(n) to store the row buffers and final output string.
//
// Data Structures:
//   - `std::vector<std::string>` of size `numRows`: one bucket per zigzag row
//     preserving left-to-right character order within each horizontal line.
//   - `std::string`: pre-reserved output buffer assembled from all row buckets.
//
// Real-World Applications:
//   - Rail-fence transposition ciphers and lightweight stream interleaving.
//   - Forward error correction (FEC) block interleavers in wireless and storage
//     channels to spread burst errors across codewords.
//   - Row-major / serpentine scan-line reordering in display controllers and
//     sensor readout pipelines.
//
// Related LeetCode Problems:
//   - #54 Spiral Matrix [done] (2D boundary-turning traversal simulation)
//   - #498 Diagonal Traverse [done] (alternating diagonal direction traversal)
//   - #1329 Sort the Matrix Diagonally [done] (grouping elements by diagonal buckets)
//   - #2075 Decode the Slanted Ciphertext [new] (inverse slanted-grid transposition)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Converts `s` into its zigzag row-major representation across `num_rows`.
  // Invariant: `row` always stays within `[0, num_rows - 1]`, reversing travel
  // direction whenever it hits either boundary.
  // Time: O(n), Space: O(n), where n = s.size().
  std::string convert(std::string_view s, int num_rows) {
    // Base cases: a single row or fewer characters than rows yields the input
    // unchanged without allocating intermediate row buckets.
    if (num_rows <= 1 || s.size() <= static_cast<std::size_t>(num_rows)) {
      return std::string(s);
    }

    const auto row_count = static_cast<std::size_t>(num_rows);
    std::vector<std::string> rows(row_count);

    std::size_t row = 0uz;
    // Starts false so the first iteration at `row == 0` flips it to true
    // (moving downward), keeping top and bottom boundary handling symmetric.
    bool going_down = false;

    for (const char ch : s) {
      rows[row].push_back(ch);

      // Reverse vertical direction upon touching the top or bottom row.
      if (row == 0uz || row == row_count - 1uz) {
        going_down = !going_down;
      }

      row = going_down ? (row + 1uz) : (row - 1uz);
    }

    // Assemble the final string in a single pre-sized allocation.
    std::string result;
    result.reserve(s.size());
    for (const std::string& line : rows) {
      result.append(line);
    }
    return result;
  }
};

LC_PROBLEM(6, "Zigzag Conversion", "string", ctx) {
  Solution sol;

  ctx.test("example1_three_rows", [&] {
    ctx.expect_eq(sol.convert("PAYPALISHIRING", 3), "PAHNAPLSIIGYIR");
  });

  ctx.test("example2_four_rows", [&] {
    ctx.expect_eq(sol.convert("PAYPALISHIRING", 4), "PINALSIGYAHRPI");
  });

  ctx.test("single_row", [&] {
    ctx.expect_eq(sol.convert("PAYPALISHIRING", 1), "PAYPALISHIRING");
  });

  ctx.test("two_rows", [&] {
    ctx.expect_eq(sol.convert("ABCDEF", 2), "ACEBDF");
  });

  ctx.test("rows_exceed_length", [&] {
    ctx.expect_eq(sol.convert("AB", 5), "AB");
  });

  ctx.test("single_char_and_empty", [&] {
    ctx.expect_eq(sol.convert("A", 1), "A");
    ctx.expect_eq(sol.convert("", 3), "");
  });
}

}  // namespace
