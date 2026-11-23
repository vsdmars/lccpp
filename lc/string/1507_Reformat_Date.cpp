// LeetCode 1507: Reformat Date
// https://leetcode.com/problems/reformat-date/
//
// Problem:
//   Given a valid date string in the format "Day Month Year", where:
//     - Day is in {"1st", "2nd", "3rd", "4th", ..., "30th", "31st"},
//     - Month is a 3-letter English abbreviation ("Jan" .. "Dec"),
//     - Year is a 4-digit integer in [1900, 2100],
//   convert the string to ISO-8601 format "YYYY-MM-DD" with zero-padded
//   2-digit month and day fields.
//
// Examples:
//   Example 1:
//     Input:  date = "20th Oct 2052"
//     Output: "2052-10-20"
//     Explanation:
//       - With `n = 13`, slice `year = date.substr(9, 4) = "2052"` and look up
//         `date.substr(5, 3) = "Oct"` in `kMonthTable` to get `"10"`.
//       - Since `n == 13` (`kTwoDigitDayInputLength`), extract the two-digit day
//         `date.substr(0, 2) = "20"`.
//       - Append `"2052"`, `'-'`, `"10"`, `'-'`, and `"20"` into the reserved
//         buffer to return `"2052-10-20"`.
//
//   Example 2:
//     Input:  date = "6th Jun 1933"
//     Output: "1933-06-06"
//     Explanation:
//       - With `n = 12`, slice `year = date.substr(8, 4) = "1933"` and map
//         `date.substr(4, 3) = "Jun"` via `kMonthTable` to `"06"`.
//       - Because `n != 13`, zero-pad the single-digit day by appending `'0'` and
//         `date[0] = '6'` to form `"06"`.
//       - Return the assembled ISO date string `"1933-06-06"`.
//
//   Example 3:
//     Input:  date = "1st Jan 1900"
//     Output: "1900-01-01"
//     Explanation:
//       - With `n = 12`, slice `year = date.substr(8, 4) = "1900"` and map
//         `date.substr(4, 3) = "Jan"` via `kMonthTable` to `"01"`.
//       - Because `n != 13`, zero-pad `date[0] = '1'` to `"01"` and return
//         `"1900-01-01"`.
//
// Algorithm:
//   Exploit the fixed-width layout of the tail (" MMM YYYY" is always 9
//   characters):
//     - The 4-digit year occupies the last 4 bytes `date.substr(n - 4, 4)`.
//     - The 3-letter month occupies `date.substr(n - 8, 3)` and maps via a
//       compile-time lookup table to its 2-digit string ("01" .. "12").
//     - The day has 2 digits when `date.size() == 13` (e.g., "20th Oct 2052")
//       and 1 digit when `date.size() == 12` (e.g., "6th Jun 1933"), in which
//       case a leading '0' is prepended.
//   Assemble the 10-character "YYYY-MM-DD" string into a pre-reserved buffer.
//   Time Complexity:  O(1).
//   Space Complexity: O(1).
//
// Data Structures:
//   - std::array<std::pair<std::string_view, std::string_view>, 12>: compile-
//     time table mapping 3-letter month abbreviations to 2-digit strings with
//     zero heap allocation.
//   - std::string_view: zero-copy slicing of the year, month, and day fields.
//
// Real-World Applications:
//   - ETL pipelines, log ingestors, and CSV/HTML scrapers normalizing human-
//     readable timestamps into sortable ISO-8601 dates.
//   - Database migration scripts canonicalizing legacy date formats before
//     indexing.
//
// Related LeetCode Problems:
//   - #1154 Day of the Year [new] (parsing ISO-8601 dates and calendar math)
//   - #1360 Number of Days Between Two Dates [new] (date parsing and calendar arithmetic)
//   - #1185 Day of the Week [new] (calendar date calculations)
//   - #2224 Minimum Number of Operations to Convert Time [new] (fixed-format timestamp parsing)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

namespace {

constexpr std::size_t kIsoDateLength = 10uz;
constexpr std::size_t kTwoDigitDayInputLength = 13uz;

// Compile-time mapping from 3-letter English month abbreviations to MM strings.
constexpr std::array<std::pair<std::string_view, std::string_view>, 12>
    kMonthTable = {{
        {"Jan", "01"},
        {"Feb", "02"},
        {"Mar", "03"},
        {"Apr", "04"},
        {"May", "05"},
        {"Jun", "06"},
        {"Jul", "07"},
        {"Aug", "08"},
        {"Sep", "09"},
        {"Oct", "10"},
        {"Nov", "11"},
        {"Dec", "12"},
    }};

class Solution {
 public:
  // Reformats a valid "Day Month Year" date string into "YYYY-MM-DD".
  //
  // Time Complexity:  O(1)
  // Space Complexity: O(1)
  std::string reformatDate(std::string_view date) {
    const std::size_t n = date.size();
    // Tail layout is always "... <MMM> <YYYY>", so offsets from the end are
    // invariant regardless of whether the day has 1 or 2 digits.
    const std::string_view year = date.substr(n - 4uz, 4uz);
    const std::string_view month = month_to_number(date.substr(n - 8uz, 3uz));

    std::string result;
    result.reserve(kIsoDateLength);
    result.append(year);
    result.push_back('-');
    result.append(month);
    result.push_back('-');

    if (n == kTwoDigitDayInputLength) {
      // Two-digit day (e.g., "20th Oct 2052"): copy both leading digits.
      result.append(date.substr(0uz, 2uz));
    } else {
      // Single-digit day (e.g., "6th Jun 1933"): zero-pad the leading digit.
      result.push_back('0');
      result.push_back(date[0uz]);
    }

    return result;
  }

 private:
  // Looks up the 2-digit month string for a 3-letter abbreviation.
  static std::string_view month_to_number(std::string_view abbr) {
    for (const auto& [name, num] : kMonthTable) {
      if (name == abbr) {
        return num;
      }
    }
    return "00";
  }
};

LC_PROBLEM(1507, "Reformat Date", "string", ctx) {
  ctx.test("example_1_build_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.reformatDate("20th Oct 2052"), std::string("2052-10-20"));
  });

  ctx.test("example_2_single_digit_day", [&] {
    Solution sol;
    ctx.expect_eq(sol.reformatDate("6th Jun 1933"), std::string("1933-06-06"));
  });

  ctx.test("example_3", [&] {
    Solution sol;
    ctx.expect_eq(sol.reformatDate("26th May 1960"), std::string("1960-05-26"));
  });

  ctx.test("boundary_years_and_suffixes", [&] {
    Solution sol;
    ctx.expect_eq(sol.reformatDate("1st Jan 1900"), std::string("1900-01-01"));
    ctx.expect_eq(sol.reformatDate("2nd Feb 2000"), std::string("2000-02-02"));
    ctx.expect_eq(sol.reformatDate("3rd Mar 1999"), std::string("1999-03-03"));
    ctx.expect_eq(sol.reformatDate("9th Sep 2024"), std::string("2024-09-09"));
    ctx.expect_eq(sol.reformatDate("10th Nov 2024"), std::string("2024-11-10"));
    ctx.expect_eq(sol.reformatDate("31st Dec 2100"), std::string("2100-12-31"));
  });
}

}  // namespace
