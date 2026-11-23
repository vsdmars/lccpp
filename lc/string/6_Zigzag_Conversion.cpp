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
// Examples:
//   Example 1:
//     Input:  s = "PAYPALISHIRING", numRows = 3
//     Output: "PAHNAPLSIIGYIR"
//     Explanation:
//       - Allocate 3 row buffers with row = 0 and going_down = false; at row = 0,
//         going_down flips to true and places 'P', 'A', 'Y' into rows 0, 1, 2.
//       - Upon hitting row = 2 (num_rows - 1), going_down flips to false so 'P'
//         goes into row 1, bouncing back down at row = 0 for 'A' and continuing
//         across all 14 characters.
//       - After the pass, the buckets hold rows[0] = "PAHN", rows[1] = "APLSIIG",
//         and rows[2] = "YIR", which concatenate to "PAHNAPLSIIGYIR".
//
//   Example 2:
//     Input:  s = "PAYPALISHIRING", numRows = 4
//     Output: "PINALSIGYAHRPI"
//     Explanation:
//       - Allocate 4 row buffers and traverse downward through rows 0..3 for
//         "PAYP", then reverse direction at row 3 to place "AL" into rows 2 and 1.
//       - Bounce at row 0 to place "ISHI" down rows 0..3, then "RI" up rows 2..1,
//         and finally "NG" into rows 0..1.
//       - Concatenate the four buckets rows[0] = "PIN", rows[1] = "ALSIG", rows[2]
//         = "YAHR", and rows[3] = "PI" to return "PINALSIGYAHRPI".
//
//   Example 3:
//     Input:  s = "AB", numRows = 5
//     Output: "AB"
//     Explanation:
//       - Evaluate the base-case guard num_rows <= 1 || s.size() <=
//         static_cast<std::size_t>(num_rows).
//       - Because s.size() = 2 <= 5, no zigzag folding occurs and the function
//         immediately returns "AB" without allocating row buffers.
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
