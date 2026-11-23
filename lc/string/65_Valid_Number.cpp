// LeetCode 65: Valid Number
// https://leetcode.com/problems/valid-number/
//
// Problem:
//   Determine whether a given string `s` can be interpreted as a valid decimal
//   number. A valid number may have optional leading/trailing spaces (in the
//   classic specification supported by the reference DFA), an optional leading
//   sign ('+' or '-'), an integer or decimal mantissa (digits, digits+'.',
//   '.'+digits, or digits+'.'+digits), and an optional exponent ('e' or 'E')
//   followed by an optional sign and one or more digits.
//
// Algorithm:
//   Model the lexical grammar of a decimal number as a 9-state Deterministic
//   Finite Automaton (DFA). Classify each character into one of five input
//   symbols (blank, sign, digit, dot, exponent) or invalid. Starting from the
//   initial state (state 1), follow the transition table for every character;
//   any missing transition immediately rejects the string. After consuming the
//   entire input, accept if and only if the final state is one of the four
//   accepting states (integer digits, fractional digits, exponent digits, or
//   trailing spaces after a valid number).
//   Time Complexity:  O(N), where N is the length of `s` (single linear scan).
//   Space Complexity: O(1), using a fixed constexpr transition table.
//
// Data Structures:
//   - Fixed 2D `std::array<std::array<int, 5>, 10>` DFA transition table:
//     maps (current_state, char_class) -> next_state (0 denotes dead/reject).
//   - Fixed `std::array<bool, 10>` accepting-state lookup table.
//
// Real-World Applications:
//   - Lexical analyzers (lexers/tokenizers) in compilers and interpreters
//     (e.g., C/C++, Python, JSON, and SQL numeric literal scanners).
//   - Protocol and configuration parsers validating numeric fields without
//     triggering exceptions or locale-dependent `strtod` edge cases.
//   - Form-input and spreadsheet cell validation engines.
//
// Related LeetCode Problems:
//   - #8 String to Integer [done] (atoi) (whitespace, sign, and digit parsing)
//   - #67 Add Binary [done] (character-by-character numeric string processing)
//   - #165 Compare Version Numbers [done] (structured numeric token scanning)
//   - #468 Validate IP Address [done] (strict multi-part string format validation)
//   - #1023 Camelcase Matching [new] (finite-state string pattern matching)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace {

// Input alphabet classes for the decimal-number DFA.
enum class CharClass : std::uint8_t {
  kBlank = 0,
  kSign = 1,
  kDigit = 2,
  kDot = 3,
  kExp = 4,
  kInvalid = 5,
};

constexpr std::size_t kNumStates = 10uz;
constexpr std::size_t kNumClasses = 5uz;

// DFA state meanings:
//   0: Dead / reject state.
//   1: Initial state / leading spaces.
//   2: Leading sign ('+' or '-') before mantissa.
//   3: Mantissa integer digits (accepting).
//   4: Leading decimal point '.' with no digits seen yet.
//   5: Mantissa after '.' with at least one digit seen (accepting).
//   6: Exponent marker ('e' or 'E') seen; expects sign or digit.
//   7: Sign after exponent marker; expects digit.
//   8: Exponent digits (accepting).
//   9: Trailing spaces after a valid number (accepting).
constexpr std::array<std::array<int, kNumClasses>, kNumStates> kTransitions = {{
    // Blank, Sign, Digit, Dot, Exp
    {0, 0, 0, 0, 0},  // 0: dead
    {1, 2, 3, 4, 0},  // 1: start / leading spaces
    {0, 0, 3, 4, 0},  // 2: sign before mantissa
    {9, 0, 3, 5, 6},  // 3: integer digits (accept)
    {0, 0, 5, 0, 0},  // 4: leading dot without digits
    {9, 0, 5, 0, 6},  // 5: digits with dot (accept)
    {0, 7, 8, 0, 0},  // 6: after 'e'/'E'
    {0, 0, 8, 0, 0},  // 7: sign after 'e'/'E'
    {9, 0, 8, 0, 0},  // 8: exponent digits (accept)
    {9, 0, 0, 0, 0},  // 9: trailing spaces (accept)
}};

constexpr std::array<bool, kNumStates> kAccepting = {
    false,  // 0
    false,  // 1
    false,  // 2
    true,   // 3: integer digits
    false,  // 4
    true,   // 5: fractional digits / trailing dot after digits
    false,  // 6
    false,  // 7
    true,   // 8: exponent digits
    true,   // 9: trailing spaces
};

class Solution {
 public:
  // Validates whether `s` represents a well-formed decimal number using a
  // table-driven DFA.
  // Time: O(N), Space: O(1).
  bool isNumber(std::string_view s) {
    int state = 1;

    for (const char ch : s) {
      const CharClass cls = classify(ch);
      if (cls == CharClass::kInvalid) {
        return false;
      }

      // Advance according to the DFA transition table; state 0 means reject.
      state = kTransitions[static_cast<std::size_t>(state)]
                          [static_cast<std::size_t>(cls)];
      if (state == 0) {
        return false;
      }
    }

    // Only complete numeric forms (or trailing spaces after one) are valid.
    return kAccepting[static_cast<std::size_t>(state)];
  }

 private:
  // Maps a raw ASCII character to its DFA input symbol class.
  static CharClass classify(char ch) {
    if (ch >= '0' && ch <= '9') {
      return CharClass::kDigit;
    }
    if (ch == ' ') {
      return CharClass::kBlank;
    }
    if (ch == '+' || ch == '-') {
      return CharClass::kSign;
    }
    if (ch == '.') {
      return CharClass::kDot;
    }
    if (ch == 'e' || ch == 'E') {
      return CharClass::kExp;
    }
    return CharClass::kInvalid;
  }
};

LC_PROBLEM(65, "Valid Number", "string", ctx) {
  Solution sol;

  ctx.test("valid integers and decimals", [&] {
    ctx.expect_true(sol.isNumber("0"));
    ctx.expect_true(sol.isNumber(" 0.1 "));
    ctx.expect_true(sol.isNumber("1 "));
    ctx.expect_true(sol.isNumber(".1"));
    ctx.expect_true(sol.isNumber("3."));
    ctx.expect_true(sol.isNumber("+.8"));
    ctx.expect_true(sol.isNumber("-123.456"));
  });

  ctx.test("valid scientific notation", [&] {
    ctx.expect_true(sol.isNumber("2e10"));
    ctx.expect_true(sol.isNumber("2E10"));
    ctx.expect_true(sol.isNumber(" -90e3   "));
    ctx.expect_true(sol.isNumber(" 6e-1"));
    ctx.expect_true(sol.isNumber("53.5e93"));
    ctx.expect_true(sol.isNumber("46.e3"));
    ctx.expect_true(sol.isNumber("-.7e+0435"));
  });

  ctx.test("invalid numbers", [&] {
    ctx.expect_false(sol.isNumber("abc"));
    ctx.expect_false(sol.isNumber("1 a"));
    ctx.expect_false(sol.isNumber(" 1e"));
    ctx.expect_false(sol.isNumber("e3"));
    ctx.expect_false(sol.isNumber(" 99e2.5 "));
    ctx.expect_false(sol.isNumber(" --6 "));
    ctx.expect_false(sol.isNumber("-+3"));
    ctx.expect_false(sol.isNumber("95a54e53"));
  });

  ctx.test("edge cases", [&] {
    ctx.expect_false(sol.isNumber(""));
    ctx.expect_false(sol.isNumber("   "));
    ctx.expect_false(sol.isNumber("."));
    ctx.expect_false(sol.isNumber("+."));
    ctx.expect_false(sol.isNumber(".e1"));
    ctx.expect_false(sol.isNumber("1e+"));
    ctx.expect_false(sol.isNumber("1 2"));
  });
}

}  // namespace
