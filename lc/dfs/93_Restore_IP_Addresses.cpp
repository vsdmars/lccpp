// LeetCode 93: Restore IP Addresses
// https://leetcode.com/problems/restore-ip-addresses/
//
// Problem:
//   Given a string `s` containing only ASCII digits, return all possible valid
//   IPv4 addresses that can be formed by inserting exactly three dots into `s`
//   without reordering or removing any digits. A valid IPv4 address consists
//   of four octets, each in [0, 255] with no leading zeros (except the single
//   digit "0" itself).
//
// Examples:
//   Example 1:
//     Input:  s = "25525511135"
//     Output: ["255.255.11.135", "255.255.111.35"]
//     Explanation:
//       - Initial length check passes because s.size() = 11 is within [4, 12], so
//         dfs starts at index 0 with 4 remaining octets.
//       - At start = 0, len = 1 ("2") leaves 10 digits for 3 octets (10 > 9,
//         pruned) and len = 2 ("25") reaches start = 2 where "525" > 255 breaks, so
//         only len = 3 ("255") advances to start = 3.
//       - At start = 3 with 8 digits and 3 octets left, len = 1 is pruned (7 > 6),
//         len = 2 fails at start = 5 ("511" > 255), and len = 3 ("255") advances to
//         start = 6.
//       - At start = 6 ("11135") with 2 octets left, len = 2 ("11") pairs with
//         start = 8 len = 3 ("135") to emit "255.255.11.135", and len = 3 ("111")
//         pairs with start = 9 len = 2 ("35") to emit "255.255.111.35".
//
//   Example 2:
//     Input:  s = "101023"
//     Output: ["1.0.10.23", "1.0.102.3", "10.1.0.23", "10.10.2.3", "101.0.2.3"]
//     Explanation:
//       - Choosing "1" at start = 0 forces start = 1 to take only "0" (breaking the
//         loop at len = 2 because s[1] == '0'), then "10"+"23" and "102"+"3" emit
//         "1.0.10.23" and "1.0.102.3".
//       - Choosing "10" at start = 0 explores start = 2 with "1" followed by "0"
//         (again capped at len = 1 by s[3] == '0') and "23", emitting "10.1.0.23".
//       - Still under "10" at start = 0, taking "10" at start = 2 followed by
//         single digits "2" and "3" emits "10.10.2.3".
//       - Choosing "101" at start = 0 hits s[3] == '0' so only "0" is valid at
//         start = 3, followed by "2" and "3" to emit "101.0.2.3".
//
//   Example 3:
//     Input:  s = "999999999999"
//     Output: []
//     Explanation:
//       - Initial length check passes with s.size() = 12, entering dfs at start = 0
//         with rem_octets = 4 and rem_digits = 12.
//       - Branches with len = 1 (rem_digits = 11 > 9) and len = 2 (rem_digits = 10
//         > 9) fail the remaining-digit feasibility check and return immediately.
//       - Trying len = 3 parses "999" as 999 > 255, breaking out of the loop
//         without recursing and returning an empty vector [].
//
// Algorithm:
//   Depth-First Search (DFS) backtracking with remaining-length pruning.
//   At each recursion level we choose the next octet (length 1, 2, or 3)
//   starting at index `start`. Before recursing, we prune any branch where:
//     1. Remaining characters cannot fit in the remaining octets
//        (fewer than `rem_octets` or more than `3 * rem_octets`).
//     2. The candidate octet has length > 1 and starts with '0' (leading zero).
//     3. The candidate octet's numeric value exceeds 255.
//   When four octets have been selected and `start == s.size()`, join the four
//   segments with '.' and emit the IP address.
//   Time Complexity:  O(1) bounded by at most 3^4 = 81 leaf configurations (at
//                     most 27 valid dot placements, each formatted in O(1)
//                     since valid inputs have length <= 12).
//   Space Complexity: O(1) recursion depth (at most 4 frames) and fixed-size
//                     octet buffer, excluding the returned output list.
//
// Data Structures:
//   - Octet path buffer (`std::vector<std::string_view>`): holds up to 4
//     zero-copy slices into `s` along the active DFS path.
//   - Result list (`std::vector<std::string>`): collects formatted IPv4 strings.
//
// Real-World Applications:
//   - Network log forensics and packet-capture recovery when delimiters are
//     stripped or corrupted in telemetry streams.
//   - Grammar-constrained string segmentation in parsers, compilers, and
//     OCR post-processing pipelines (e.g., recovering dates, IPv4s, or IDs).
//   - Input validation and ambiguity detection in configuration analyzers.
//
// Related LeetCode Problems:
//   - #131 Palindrome Partitioning [done] (backtracking string segmentation)
//   - #140 Word Break II [done] (DFS string partitioning with dictionary checks)
//   - #468 Validate IP Address [done] (IPv4 and IPv6 syntax validation)
//   - #17 Letter Combinations of a Phone Number [done] (bounded-depth DFS)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::size_t kNumOctets = 4uz;
constexpr std::size_t kMaxOctetLen = 3uz;
constexpr int kMaxOctetValue = 255;

class Solution {
 public:
  // Returns all valid IPv4 addresses obtainable by inserting three dots into
  // digit-only string `s`.
  // Time: O(1) (at most 3^4 branches, max length 12), Space: O(1) auxiliary.
  std::vector<std::string> restoreIpAddresses(std::string_view s) {
    std::vector<std::string> result;
    // Four octets require between 4 and 12 digits inclusive.
    if (s.size() < kNumOctets || s.size() > (kNumOctets * kMaxOctetLen)) {
      return result;
    }

    std::vector<std::string_view> segments;
    segments.reserve(kNumOctets);
    dfs(s, 0uz, segments, result);
    return result;
  }

 private:
  // Parses a 1..3 digit non-empty view into its integer value.
  static int parseOctet(std::string_view segment) {
    int value = 0;
    for (const char ch : segment) {
      value = (value * 10) + static_cast<int>(ch - '0');
    }
    return value;
  }

  // Joins the 4 octet views into a dotted IPv4 string in a single allocation.
  static std::string joinOctets(const std::vector<std::string_view>& segments,
                                std::size_t digit_count) {
    std::string ip;
    ip.reserve(digit_count + (kNumOctets - 1uz));
    for (std::size_t i = 0uz; i < segments.size(); ++i) {
      if (i > 0uz) {
        ip.push_back('.');
      }
      ip.append(segments[i]);
    }
    return ip;
  }

  // Backtracking DFS selecting `segments[segments.size()]` starting at `start`.
  static void dfs(std::string_view s, std::size_t start,
                  std::vector<std::string_view>& segments,
                  std::vector<std::string>& result) {
    const std::size_t rem_octets = kNumOctets - segments.size();
    const std::size_t rem_digits = s.size() - start;

    // Base case: all 4 octets chosen; accept only if every digit was consumed.
    if (rem_octets == 0uz) {
      if (rem_digits == 0uz) {
        result.push_back(joinOctets(segments, s.size()));
      }
      return;
    }

    // Feasibility pruning: remaining digits must fit within [1, 3] per octet.
    if (rem_digits < rem_octets || rem_digits > (rem_octets * kMaxOctetLen)) {
      return;
    }

    const std::size_t max_len = std::min(kMaxOctetLen, rem_digits);
    for (std::size_t len = 1uz; len <= max_len; ++len) {
      // Multi-digit octets cannot have a leading '0'; longer lengths also fail.
      if (len > 1uz && s[start] == '0') {
        break;
      }

      const std::string_view octet = s.substr(start, len);
      // Values grow monotonically with `len`, so > 255 stops this loop early.
      if (parseOctet(octet) > kMaxOctetValue) {
        break;
      }

      segments.push_back(octet);
      dfs(s, start + len, segments, result);
      segments.pop_back();
    }
  }
};

LC_PROBLEM(93, "Restore IP Addresses", "dfs", ctx) {
  Solution sol;

  ctx.test("example_1_25525511135", [&] {
    auto actual = sol.restoreIpAddresses("25525511135");
    std::vector<std::string> expected = {"255.255.11.135", "255.255.111.35"};
    std::ranges::sort(actual);
    std::ranges::sort(expected);
    ctx.expect_eq(actual, expected);
  });

  ctx.test("example_2_0000", [&] {
    const auto actual = sol.restoreIpAddresses("0000");
    const std::vector<std::string> expected = {"0.0.0.0"};
    ctx.expect_eq(actual, expected);
  });

  ctx.test("example_3_101023", [&] {
    auto actual = sol.restoreIpAddresses("101023");
    std::vector<std::string> expected = {
        "1.0.10.23", "1.0.102.3", "10.1.0.23", "10.10.2.3", "101.0.2.3",
    };
    std::ranges::sort(actual);
    std::ranges::sort(expected);
    ctx.expect_eq(actual, expected);
  });

  ctx.test("length_out_of_bounds_and_invalid", [&] {
    ctx.expect_true(sol.restoreIpAddresses("").empty());
    ctx.expect_true(sol.restoreIpAddresses("123").empty());
    ctx.expect_true(sol.restoreIpAddresses("1111111111111").empty());
    ctx.expect_true(sol.restoreIpAddresses("999999999999").empty());
  });

  ctx.test("all_255_max_valid", [&] {
    const auto actual = sol.restoreIpAddresses("255255255255");
    const std::vector<std::string> expected = {"255.255.255.255"};
    ctx.expect_eq(actual, expected);
  });
}

}  // namespace
