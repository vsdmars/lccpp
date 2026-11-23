// LeetCode 68: Text Justification
// https://leetcode.com/problems/text-justification/
//
// Problem:
//   Given an array of words and a target line width `maxWidth`, format the text
//   so that every line has exact length `maxWidth` and is fully (left and right)
//   justified. Pack as many words as possible greedily onto each line. Extra
//   spaces between words must be distributed as evenly as possible, with any
//   remainder spaces assigned one-by-one to the leftmost gaps. A line with only
//   one word and the final line of text must be left-justified (single spaces
//   between words, padded on the right with trailing spaces to `maxWidth`).
//   Also includes the explicit-newline paragraph variant from the source file.
//
// Algorithm:
//   Use a greedy two-pointer line-packing pass:
//   1. Starting at word index `i`, advance `j` while the sum of word lengths
//      plus the minimum single-space separators `(j - i)` does not exceed
//      `maxWidth`.
//   2. Format the packed slice `words[i..j-1]`:
//      - If it is the last line (`j == words.size()`) or contains only a single
//        word (`j - i == 1`), join words with single spaces and right-pad with
//        spaces to `maxWidth`.
//      - Otherwise, let `gaps = (j - i) - 1` and `total_spaces = maxWidth -
//        words_len`. Assign `total_spaces / gaps` spaces to every gap, plus one
//        extra space to the first `total_spaces % gaps` gaps.
//   3. Advance `i = j` until all words are placed.
//   Time Complexity:  O(C), where C is the total number of characters emitted
//                     across all output lines (each word and space written once).
//   Space Complexity: O(C) for the formatted output lines (O(1) extra scratch).
//
// Data Structures:
//   - `std::vector<std::string>` storing the input words and output lines.
//   - Two `std::size_t` slice indices `[start, end)` defining the current line
//     without copying intermediate word lists.
//
// Real-World Applications:
//   - Rich-text layout engines, word processors, PDF/TeX typesetters, and
//     browser CSS `text-align: justify` line-breaking pipelines.
//   - Terminal/CLI documentation formatters, man-page renderers, and receipt
//     or fixed-width report printers.
//   - Subtitle and e-reader pagination engines.
//
// Related LeetCode Problems:
//   - #151 Reverse Words In A String [done] (whitespace tokenization and reassembly)
//   - #186 Reverse Words In A String II [done] (word-boundary manipulation)
//   - #418 Sentence Screen Fitting [done] (packing words into fixed-width rows)
//   - #1592 Rearrange Spaces Between Words [new] (even space distribution on one line)
//   - #1694 Reformat Phone Number [new] (greedy fixed-width text formatting)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Packs and justifies `words` into lines of exact width `max_width`.
  // Time: O(total output chars), Space: O(total output chars).
  std::vector<std::string> fullJustify(const std::vector<std::string>& words,
                                       int max_width) {
    return justifyWords(words, static_cast<std::size_t>(max_width),
                        /*justify_last_line=*/false);
  }

  // Follow-up variant from the source file: justifies text containing explicit
  // '\n' paragraph breaks. Paragraphs terminated by '\n' are full-justified on
  // their final line, while the trailing paragraph (if non-empty) is left-
  // justified on its final line.
  std::vector<std::string> fullJustifyWithNewlines(std::string_view text,
                                                   int max_width) {
    const auto width = static_cast<std::size_t>(max_width);
    std::vector<std::string> result;

    std::size_t pos = 0uz;
    while (pos <= text.size()) {
      const std::size_t nl = text.find('\n', pos);
      const bool has_newline = (nl != std::string_view::npos);
      const std::string_view segment =
          has_newline ? text.substr(pos, nl - pos) : text.substr(pos);

      const std::vector<std::string> para_words = splitWhitespace(segment);
      if (!para_words.empty()) {
        std::vector<std::string> lines =
            justifyWords(para_words, width, /*justify_last_line=*/has_newline);
        for (std::string& line : lines) {
          result.push_back(std::move(line));
        }
      }

      if (!has_newline) {
        break;
      }
      pos = nl + 1uz;
    }

    return result;
  }

 private:
  // Splits `text` on ASCII spaces/tabs into non-empty word tokens.
  static std::vector<std::string> splitWhitespace(std::string_view text) {
    std::vector<std::string> tokens;
    std::size_t i = 0uz;
    while (i < text.size()) {
      while (i < text.size() && (text[i] == ' ' || text[i] == '\t')) {
        ++i;
      }
      const std::size_t start = i;
      while (i < text.size() && text[i] != ' ' && text[i] != '\t') {
        ++i;
      }
      if (i > start) {
        tokens.emplace_back(text.substr(start, i - start));
      }
    }
    return tokens;
  }

  // Formats one packed line `words[start..end)` to exact length `width`.
  static std::string formatLine(const std::vector<std::string>& words,
                                std::size_t start, std::size_t end,
                                std::size_t words_len, std::size_t width,
                                bool left_align) {
    const std::size_t count = end - start;
    std::string line;
    line.reserve(width);

    // Single-word lines and left-aligned final lines use single spaces plus
    // trailing space padding up to `width`.
    if (left_align || count == 1uz) {
      for (std::size_t k = start; k < end; ++k) {
        if (k > start) {
          line.push_back(' ');
        }
        line.append(words[k]);
      }
      line.append(width - line.size(), ' ');
      return line;
    }

    // Full justification: distribute `total_spaces` across `gaps` interior
    // slots, assigning the `extra_spaces` remainder to the leftmost slots.
    const std::size_t gaps = count - 1uz;
    const std::size_t total_spaces = width - words_len;
    const std::size_t base_spaces = total_spaces / gaps;
    const std::size_t extra_spaces = total_spaces % gaps;

    for (std::size_t k = start; k < end; ++k) {
      line.append(words[k]);
      if (k + 1uz < end) {
        const std::size_t gap_idx = k - start;
        const std::size_t pad = base_spaces + (gap_idx < extra_spaces ? 1uz : 0uz);
        line.append(pad, ' ');
      }
    }

    return line;
  }

  // Greedily packs `words` into lines of width `width`.
  static std::vector<std::string> justifyWords(
      const std::vector<std::string>& words, std::size_t width,
      bool justify_last_line) {
    std::vector<std::string> lines;
    std::size_t i = 0uz;

    while (i < words.size()) {
      // Greedily include words[j] while total characters + minimum 1-space
      // separators still fit inside `width`.
      std::size_t j = i;
      std::size_t words_len = 0uz;
      while (j < words.size()) {
        const std::size_t min_gaps = j - i;
        if ((words_len + words[j].size()) + min_gaps > width) {
          break;
        }
        words_len += words[j].size();
        ++j;
      }

      const bool is_last_line = (j == words.size());
      const bool left_align = is_last_line && !justify_last_line;
      lines.push_back(formatLine(words, i, j, words_len, width, left_align));
      i = j;
    }

    return lines;
  }
};

LC_PROBLEM(68, "Text Justification", "string", ctx) {
  Solution sol;

  ctx.test("leetcode example 1", [&] {
    const std::vector<std::string> words = {
        "This", "is", "an", "example", "of", "text", "justification."};
    const std::vector<std::string> expected = {
        "This    is    an",
        "example  of text",
        "justification.  ",
    };
    ctx.expect_eq(sol.fullJustify(words, 16), expected);
  });

  ctx.test("leetcode example 2", [&] {
    const std::vector<std::string> words = {"What", "must", "be",
                                            "acknowledgment", "shall", "be"};
    const std::vector<std::string> expected = {
        "What   must   be",
        "acknowledgment  ",
        "shall be        ",
    };
    ctx.expect_eq(sol.fullJustify(words, 16), expected);
  });

  ctx.test("leetcode example 3", [&] {
    const std::vector<std::string> words = {
        "Science", "is", "what", "we", "understand", "well", "enough", "to",
        "explain", "to", "a", "computer.", "Art", "is", "everything", "else",
        "we", "do"};
    const std::vector<std::string> expected = {
        "Science  is  what we",
        "understand      well",
        "enough to explain to",
        "a  computer.  Art is",
        "everything  else  we",
        "do                  ",
    };
    ctx.expect_eq(sol.fullJustify(words, 20), expected);
  });

  ctx.test("exact width and single word", [&] {
    const std::vector<std::string> words = {"hello"};
    const std::vector<std::string> expected = {"hello"};
    ctx.expect_eq(sol.fullJustify(words, 5), expected);
  });

  ctx.test("explicit newline follow-up", [&] {
    const std::vector<std::string> expected = {
        "This    is    an",
        "example  of text",
        "justification.  ",
    };
    ctx.expect_eq(
        sol.fullJustifyWithNewlines("This is an\nexample of text justification.",
                                    16),
        expected);
  });
}

}  // namespace
