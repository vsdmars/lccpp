// LeetCode 420: Strong Password Checker
// https://leetcode.com/problems/strong-password-checker/
//
// Problem:
//   A password is strong if it satisfies all three rules:
//     1. Its length is at least 6 and at most 20 characters.
//     2. It contains at least one lowercase letter, at least one uppercase
//        letter, and at least one digit.
//     3. It does not contain three repeating characters in a row.
//   In one step you may insert, delete, or replace a single character. Given a
//   string `password`, return the minimum number of steps required to make it
//   strong (or 0 if it is already strong).
//
// Algorithm:
//   First count `missing_types` in {0, 1, 2, 3} for the three required
//   character classes. Next scan all maximal runs of identical characters with
//   length L >= 3:
//     - Without deletions, breaking all triples in a run of length L requires
//       floor(L / 3) replacements. Sum these into `replacements`.
//     - When the string is longer than 20, mandatory deletions can also reduce
//       `replacements`, with efficiency determined by L mod 3:
//         * L % 3 == 0: 1 deletion saves 1 replacement (reduces 3k to 3k - 1).
//         * L % 3 == 1: 2 deletions save 1 replacement (reduces 3k+1 to 3k-1).
//         * After those reductions (or when L % 3 == 2), every 3 deletions save
//           1 replacement.
//   Split into three length regimes (let n = password.size()):
//     1. n < 6: at least (6 - n) insertions are required, and each insertion
//        can simultaneously fix a missing character class and break any run of
//        length <= 5. Answer = max(missing_types, 6 - n).
//     2. 6 <= n <= 20: replacements dominate insertions and deletions because
//        each replacement can fix both a triple and a missing type.
//        Answer = max(missing_types, replacements).
//     3. n > 20: we must perform `deletions = n - 20` deletions. Greedily
//        assign them first to mod-0 runs (1 per saved replacement), then to
//        mod-1 runs (2 per saved replacement), then 3 per saved replacement.
//        Answer = deletions + max(missing_types, max(0, remaining_replacements)).
//   Time Complexity: O(n), single linear scan over `password`.
//   Space Complexity: O(1).
//
// Data Structures:
//   Constant integer counters for missing character classes, required
//   replacements, and deletion-efficiency buckets (`one_del_runs`,
//   `two_del_runs`).
//
// Real-World Applications:
//   Password policy strength meters and minimum-edit remediation suggestions,
//   automated compliance/linting distance estimation for constrained text
//   formats, and greedy resource allocation across discrete cost tiers.
//
// Related LeetCode Problems:
//   - #72 Edit Distance [done] (general minimum insert/delete/replace distance between strings)
//   - #2299 Strong Password Checker II [new] (linear-time strong password validation)
//   - #161 One Edit Distance [done] (checking exact single-edit transformability)
//   - #680 Valid Palindrome II [done] (minimal deletion constraint checking on strings)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns the minimum number of insert, delete, or replace steps needed to
  // satisfy all three strong-password constraints.
  // Time: O(n), Space: O(1).
  int strongPasswordChecker(std::string_view password) {
    constexpr int kMinLength = 6;
    constexpr int kMaxLength = 20;

    const int n = static_cast<int>(password.size());
    const int missing_types = count_missing_types(password);

    // Scan maximal repeating runs of length >= 3 and bucket them by how many
    // deletions are needed to save the first replacement in that run.
    int replacements = 0;
    int one_del_runs = 0;  // runs with len % 3 == 0 (1 deletion saves 1 replace)
    int two_del_runs = 0;  // runs with len % 3 == 1 (2 deletions save 1 replace)

    std::size_t i = 0uz;
    while (i < password.size()) {
      std::size_t j = i;
      while ((j < password.size()) && (password[j] == password[i])) {
        ++j;
      }

      const int run_len = static_cast<int>(j - i);
      if (run_len >= 3) {
        replacements += run_len / 3;
        if ((run_len % 3) == 0) {
          ++one_del_runs;
        } else if ((run_len % 3) == 1) {
          ++two_del_runs;
        }
      }
      i = j;
    }

    // Regime 1: too short. Because n <= 5, at most one repeating run of length
    // 3..5 can exist, and insertions can fix both length, repeats, and types.
    if (n < kMinLength) {
      return std::max(missing_types, kMinLength - n);
    }

    // Regime 2: valid length. Replacements simultaneously resolve triples and
    // missing character classes with no length penalty.
    if (n <= kMaxLength) {
      return std::max(missing_types, replacements);
    }

    // Regime 3: too long. Spend mandatory deletions greedily from highest to
    // lowest replacement-saving efficiency (1-for-1, then 2-for-1, then 3-for-1).
    const int deletions = n - kMaxLength;
    int rem_del = deletions;

    // Tier 1: spend 1 deletion each on runs with length % 3 == 0.
    const int use_one = std::min(rem_del, one_del_runs);
    replacements -= use_one;
    rem_del -= use_one;

    // Tier 2: spend 2 deletions each on runs with length % 3 == 1.
    const int use_two = std::min(rem_del, two_del_runs * 2);
    replacements -= use_two / 2;
    rem_del -= use_two;

    // Tier 3: every 3 remaining deletions save 1 replacement on any remaining
    // run of length >= 3 (all active runs now satisfy reduced_len % 3 == 2).
    replacements -= rem_del / 3;

    return deletions + std::max({missing_types, 0, replacements});
  }

 private:
  // Counts how many of the three required character classes (lowercase,
  // uppercase, digit) are absent from `password`.
  static int count_missing_types(std::string_view password) {
    bool has_lower = false;
    bool has_upper = false;
    bool has_digit = false;

    for (const char c : password) {
      if ((c >= 'a') && (c <= 'z')) {
        has_lower = true;
      } else if ((c >= 'A') && (c <= 'Z')) {
        has_upper = true;
      } else if ((c >= '0') && (c <= '9')) {
        has_digit = true;
      }
    }

    return static_cast<int>(!has_lower) + static_cast<int>(!has_upper) +
           static_cast<int>(!has_digit);
  }
};

LC_PROBLEM(420, "Strong Password Checker", "math", ctx) {
  ctx.test("example_1_single_char_build", [&] {
    Solution sol;
    ctx.expect_eq(sol.strongPasswordChecker("a"), 5);
  });

  ctx.test("example_2_short_all_types", [&] {
    Solution sol;
    ctx.expect_eq(sol.strongPasswordChecker("aA1"), 3);
  });

  ctx.test("example_3_already_strong", [&] {
    Solution sol;
    ctx.expect_eq(sol.strongPasswordChecker("1337C0d3"), 0);
  });

  ctx.test("short_repeating_five_chars", [&] {
    Solution sol;
    // "aaaaa" -> insert 1 uppercase to break run, replace 1 with digit -> 2.
    ctx.expect_eq(sol.strongPasswordChecker("aaaaa"), 2);
  });

  ctx.test("valid_length_multiple_runs", [&] {
    Solution sol;
    // Length 6, missing uppercase (1), two triples (2) -> max(1, 2) = 2.
    ctx.expect_eq(sol.strongPasswordChecker("aaa111"), 2);
  });

  ctx.test("long_password_mod0_and_mod2_deletions", [&] {
    Solution sol;
    // Length 23 (3 deletions), runs of 15 and 6 (both mod 0), missing 2 types:
    // 2 deletions save 2 of 7 replacements -> 3 + max(2, 5) = 8.
    ctx.expect_eq(sol.strongPasswordChecker("bbaaaaaaaaaaaaaaacccccc"), 8);
  });

  ctx.test("long_password_mod1_deletions", [&] {
    Solution sol;
    // Length 24 (4 deletions), six runs of 4 (all mod 1), missing 2 types:
    // 4 deletions save 2 of 6 replacements -> 4 + max(2, 4) = 8.
    ctx.expect_eq(sol.strongPasswordChecker("aaaabbbbccccddddeeeeffff"), 8);
  });

  ctx.test("long_password_no_repeats", [&] {
    Solution sol;
    // Length 21 (1 deletion), missing lowercase (1), 0 repeats -> 1 + 1 = 2.
    ctx.expect_eq(sol.strongPasswordChecker("ABABABABABABABABABAB1"), 2);
  });
}

}  // namespace
