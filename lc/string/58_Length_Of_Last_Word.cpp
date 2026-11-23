// LeetCode 58: Length Of Last Word
// https://leetcode.com/problems/length-of-last-word/
//
// Problem:
//   Given a string `s` consisting of words and ASCII spaces `' '`, return the
//   length of the last word in `s`. A word is defined as a maximal contiguous
//   substring of non-space characters.
//
//   Constraints:
//   - 1 <= s.length <= 10^4
//   - s consists of only English letters and spaces `' '`
//   - There is at least one word in s
//
// Examples:
//   Example 1:
//     Input:  s = "Hello World"
//     Output: 5
//     Explanation:
//       - Initialize end = 11 (s.size()); because s[10] == 'd' is non-space, Phase
//         1 stops immediately with end = 11.
//       - Phase 2 sets start = 11 and decrements across 'd', 'l', 'r', 'o', 'W'
//         until s[start - 1] == s[5] == ' ', stopping at start = 6.
//       - Return end - start = 11 - 6 = 5.
//
//   Example 2:
//     Input:  s = "   fly me   to   the moon  "
//     Output: 4
//     Explanation:
//       - Initialize end = 27 and decrement past the two trailing spaces at indices
//         26 and 25, stopping at end = 25 where s[24] == 'n'.
//       - Set start = 25 and walk backward across 'n', 'o', 'o', 'm' until s[start
//         - 1] == s[20] == ' ', stopping at start = 21.
//       - Return end - start = 25 - 21 = 4.
//
//   Example 3:
//     Input:  s = "     "
//     Output: 0
//     Explanation:
//       - Initialize end = 5 and decrement past all five spaces at indices 4 down
//         to 0, reaching end = 0.
//       - Phase 2 starts at start = 0, so the start > 0 guard prevents any
//         decrement.
//       - Return end - start = 0 - 0 = 0.
//
// Algorithm:
//   Single reverse scan from the end of `s` without allocating or splitting:
//   1. Start an exclusive boundary index `end = s.size()` and step backward
//      past any trailing spaces (`s[end - 1] == ' '`).
//   2. Set `start = end` and step backward while `start > 0` and
//      `s[start - 1] != ' '` to locate the first character of the final word.
//   3. Return `end - start`.
//   Because the scan starts at the back, it avoids touching earlier words in
//   long inputs and allocates zero extra memory.
//
//   Time Complexity:  O(n) worst-case (e.g., a single word spanning the whole
//                     string), and O(k + t) in practice where k is the last
//                     word's length and t is the trailing-space count.
//   Space Complexity: O(1) auxiliary space.
//
// Data Structures:
//   - Two unsigned cursor indices (`end`, `start`) over a non-owning
//     `std::string_view`.
//
// Real-World Applications:
//   - Zero-copy trailing-token extraction in log parsers, shell command-line
//     autocompletion, and HTTP/SMTP header scanners.
//   - Suffix/basename length checks in streaming text and path processors.
//
// Related LeetCode Problems:
//   - #151 Reverse Words in a String [done] (whitespace trimming and word bounds)
//   - #434 Number of Segments in a String [new] (counting maximal non-space runs)
//   - #557 Reverse Words in a String III [done] (word-boundary scanning)
//   - #1816 Truncate Sentence [new] (space-delimited word prefix slicing)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns the length of the final maximal non-space substring in `s`.
  // Invariant: after Phase 1, `[end, s.size())` contains only spaces; after
  // Phase 2, `[start, end)` is the exact span of the last word.
  // Time: O(n) worst-case, Space: O(1).
  int lengthOfLastWord(std::string_view s) {
    std::size_t end = s.size();

    // Phase 1: Skip trailing spaces after the last word.
    while (end > 0uz && s[end - 1uz] == ' ') {
      --end;
    }

    // Phase 2: Measure the contiguous non-space span of the last word.
    std::size_t start = end;
    while (start > 0uz && s[start - 1uz] != ' ') {
      --start;
    }

    return static_cast<int>(end - start);
  }
};

LC_PROBLEM(58, "Length Of Last Word", "string", ctx) {
  Solution sol;

  ctx.test("examples_from_statement", [&] {
    ctx.expect_eq(sol.lengthOfLastWord("Hello World"), 5);
    ctx.expect_eq(sol.lengthOfLastWord("   fly me   to   the moon  "), 4);
    ctx.expect_eq(sol.lengthOfLastWord("luffy is still joyboy"), 6);
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.lengthOfLastWord("a"), 1);
    ctx.expect_eq(sol.lengthOfLastWord("a   "), 1);
    ctx.expect_eq(sol.lengthOfLastWord("   a"), 1);
    ctx.expect_eq(sol.lengthOfLastWord(""), 0);
    ctx.expect_eq(sol.lengthOfLastWord("     "), 0);
  });
}

}  // namespace
