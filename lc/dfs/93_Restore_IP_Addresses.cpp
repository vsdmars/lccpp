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
