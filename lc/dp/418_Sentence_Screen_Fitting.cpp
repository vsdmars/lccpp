// LeetCode 418: Sentence Screen Fitting
// https://leetcode.com/problems/sentence-screen-fitting/
//
// Problem:
//   Given a screen of dimensions `rows x cols` and a non-empty list of words
//   representing a sentence, return the number of complete times the sentence
//   can be fitted on the screen. Words cannot be split across lines, their
//   relative order must be preserved, and two consecutive words on the same
//   line must be separated by a single space.
//   Constraints: 1 <= sentence.size() <= 100, 1 <= word.size() <= 10,
//   1 <= rows, cols <= 20,000.
//
// Algorithm:
//   Concatenate all words with single spaces plus a trailing space into one
//   cyclic template string `formatted` of length L. Track a cursor `pos`
//   representing the total number of valid characters from the infinite
//   repetition of `formatted` placed onto the screen so far.
//   For each of the `rows` rows, optimistically advance `pos` by `cols`:
//     - If `formatted[pos % L] == ' '`, the row ends cleanly at a word
//       boundary, so consume the trailing space (`++pos`) because the line
//       break acts as the separator.
//     - Otherwise, `pos` landed in the middle of a word, so step backward
//       until the character immediately before `pos` is a space (or `pos`
//       reaches 0), leaving the partial word for the next row.
//   After processing all rows, `pos / L` gives the exact number of complete
//   sentences fitted.
//   Time Complexity:  O(rows * max_word_len + L), where L is total sentence
//                     length (each row backtracks at most one word length).
//   Space Complexity: O(L) to store the joined sentence string.
//
// Data Structures:
//   - `std::string` (`formatted`): flattened cyclic representation of the
//     sentence with single-space separators and a trailing space.
//   - Scalar cursor (`std::size_t pos`): cumulative character offset across
//     repeated copies of `formatted`, avoiding 32-bit overflow when
//     `rows * cols` is large.
//
// Real-World Applications:
//   - Terminal emulators, text editors, and UI layout engines computing word
//     wrapping and line pagination inside fixed-width viewports.
//   - Digital signage and LED marquee controllers estimating repeat counts
//     for scrolling ticker messages on fixed character grids.
//   - Typesetting engines (e.g., Knuth-Plass pre-pass or greedy line breakers)
//     determining page capacity for repetitive report templates.
//
// Related LeetCode Problems:
//   - #68 Text Justification [done] (greedy line-by-line word packing with space rules)
//   - #1592 Rearrange Spaces Between Words [new] (spacing and line formatting mechanics)
//   - #466 Count The Repetitions [new] (cyclic string repetition counting with cursor jumps)
//   - #1105 Filling Bookcase Shelves [new] (packing sequential items into fixed-width rows)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <vector>

namespace {

class Solution {
 public:
  // Computes how many complete times `sentence` fits on a `rows x cols` screen
  // using a greedy cyclic-string cursor.
  // Time: O(rows * max_word_len + L), Space: O(L).
  int wordsTyping(const std::vector<std::string>& sentence, int rows,
                  int cols) {
    if (sentence.empty() || rows <= 0 || cols <= 0) {
      return 0;
    }

    // Flatten the sentence into a single cyclic string terminated by a space so
    // every word — including the last — is followed by a uniform delimiter.
    std::string formatted;
    for (const std::string& word : sentence) {
      formatted += word;
      formatted.push_back(' ');
    }

    const std::size_t cycle_len = formatted.size();
    const auto num_rows = static_cast<std::size_t>(rows);
    const auto num_cols = static_cast<std::size_t>(cols);

    // `pos` is the index in the infinite repetition of `formatted` where the
    // next row will begin placing characters.
    std::size_t pos = 0uz;

    for (std::size_t row = 0uz; row < num_rows; ++row) {
      // Optimistically fill the entire width of the current row.
      pos += num_cols;

      if (formatted[pos % cycle_len] == ' ') {
        // The row ends right before a space; the line break serves as that
        // space, so skip it to start the next row at the following word.
        ++pos;
      } else {
        // The row cut through the middle of a word. Rewind until the character
        // preceding `pos` is a space (or we hit the start of the stream if a
        // word exceeds `cols`), leaving the whole word for the next row.
        while (pos > 0uz && formatted[(pos - 1uz) % cycle_len] != ' ') {
          --pos;
        }
      }
    }

    return static_cast<int>(pos / cycle_len);
  }
};

LC_PROBLEM(418, "Sentence Screen Fitting", "dp", ctx) {
  ctx.test("example_1_two_words", [&] {
    Solution sol;
    const std::vector<std::string> sentence = {"hello", "world"};
    ctx.expect_eq(sol.wordsTyping(sentence, 2, 8), 1);
  });

  ctx.test("example_2_multiple_fits", [&] {
    Solution sol;
    const std::vector<std::string> sentence = {"a", "bcd", "e"};
    ctx.expect_eq(sol.wordsTyping(sentence, 3, 6), 2);
  });

  ctx.test("example_3_four_words_tight_width", [&] {
    Solution sol;
    const std::vector<std::string> sentence = {"I", "had", "apple", "pie"};
    ctx.expect_eq(sol.wordsTyping(sentence, 4, 5), 1);
  });

  ctx.test("source_build_case", [&] {
    Solution sol;
    const std::vector<std::string> sentence = {"Iia", "had"};
    ctx.expect_eq(sol.wordsTyping(sentence, 4, 5), 2);
  });

  ctx.test("word_wider_than_screen", [&] {
    Solution sol;
    const std::vector<std::string> sentence = {"hello", "world"};
    ctx.expect_eq(sol.wordsTyping(sentence, 3, 4), 0);
  });

  ctx.test("single_word_many_repeats_per_row", [&] {
    Solution sol;
    const std::vector<std::string> sentence = {"a"};
    // Each row of width 5 fits "a a a" (3 copies); 2 rows fit 6 copies.
    ctx.expect_eq(sol.wordsTyping(sentence, 2, 5), 6);
  });
}

}  // namespace
