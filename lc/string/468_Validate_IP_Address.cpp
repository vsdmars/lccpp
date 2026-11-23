// LeetCode 468: Validate IP Address
// https://leetcode.com/problems/validate-ip-address/
//
// Problem:
//   Given a string `queryIP`, determine whether it represents a valid IPv4
//   address, a valid IPv6 address, or neither, returning `"IPv4"`, `"IPv6"`,
//   or `"Neither"` respectively.
//   - A valid IPv4 address has the form `x1.x2.x3.x4` with 4 decimal octets,
//     each in [0, 255] and without leading zeros (e.g., `"0"` is valid, while
//     `"01"` and `"00"` are invalid).
//   - A valid IPv6 address has the form `x1:x2:x3:x4:x5:x6:x7:x8` with 8
//     hexadecimal groups, each 1 to 4 characters long using digits `'0'-'9'`,
//     `'a'-'f'`, or `'A'-'F'`. Leading zeros within a 4-digit group are
//     allowed, while shorthand `"::"` compression and extra (>4) digits are
//     disallowed.
//
// Examples:
//   Example 1:
//     Input:  queryIP = "172.16.254.1"
//     Output: "IPv4"
//     Explanation:
//       - Count '.' occurrences in "172.16.254.1" and get 3, dispatching to IPv4
//         validation.
//       - Split on '.' into four std::string_view segments: ["172", "16", "254",
//         "1"].
//       - Verify each segment has length 1..3, no leading '0' when length > 1, only
//         ASCII digits, and numeric values 172, 16, 254, 1 all <= 255, returning
//         "IPv4".
//
//   Example 2:
//     Input:  queryIP = "2001:0db8:85a3:0:0:8A2E:0370:7334"
//     Output: "IPv6"
//     Explanation:
//       - Count '.' (0) and ':' (7) in the input, dispatching to IPv6 validation.
//       - Split on ':' into eight segments: ["2001", "0db8", "85a3", "0", "0",
//         "8A2E", "0370", "7334"].
//       - Verify every segment has length 1..4 and contains only hexadecimal
//         characters ('0'-'9', 'a'-'f', 'A'-'F'), returning "IPv6".
//
//   Example 3:
//     Input:  queryIP = "172.16.254.01"
//     Output: "Neither"
//     Explanation:
//       - Count 3 '.' delimiters and split into ["172", "16", "254", "01"].
//       - Pass is_valid_ipv4_segment on "172", "16", and "254", then reject "01"
//         because segment.size() > 1 && segment[0] == '0'.
//       - Return "Neither" since not all segments are valid IPv4 octets.
//
// Algorithm:
//   Dispatch by counting delimiters first:
//   1. If `queryIP` contains exactly 3 dots (`'.'`), split it into 4
//      zero-copy `std::string_view` segments and verify every segment is a
//      valid IPv4 octet (length 1..3, no leading `'0'` when length > 1, all
//      ASCII digits, numeric value <= 255).
//   2. Else if `queryIP` contains exactly 7 colons (`':'`), split it into 8
//      zero-copy `std::string_view` segments and verify every segment has
//      length 1..4 and consists solely of hexadecimal characters.
//   3. Otherwise return `"Neither"`.
//   Explicit per-character validation avoids pitfalls of generic integer
//   parsers (such as accepting leading `'+'`, whitespace, or `"0x"` prefixes).
//   Time Complexity: O(N), where N is the length of `queryIP`.
//   Space Complexity: O(1) auxiliary space (at most 4 or 8 `std::string_view`
//   slices are stored after verifying the delimiter count).
//
// Data Structures:
//   - `std::string_view`: zero-copy slices of the input string representing
//     individual IPv4 octets or IPv6 hextets.
//   - `std::vector<std::string_view>`: bounded container (size 4 or 8) holding
//     the split segments for validation.
//
// Real-World Applications:
//   - Firewall rule parsers, reverse proxies, and load balancers validating
//     endpoint addresses before binding or routing sockets.
//   - DNS resolvers and networking libraries (`inet_pton`) distinguishing IPv4
//     and IPv6 literals in configuration files and HTTP `Host` headers.
//   - Input sanitization in cloud IAM / CIDR allowlist management APIs.
//
// Related LeetCode Problems:
//   - #93 Restore IP Addresses [done] (backtracking all valid IPv4 splits of a digit string)
//   - #65 Valid Number [done] (strict lexical validation of numeric string formats)
//   - #8 String to Integer [done] (atoi) (careful ASCII-to-integer parsing with edge cases)
//   - #751 IP to CIDR [done] (converting IPv4 addresses into CIDR network blocks)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Classifies `query_ip` as `"IPv4"`, `"IPv6"`, or `"Neither"`.
  // Time: O(N), Space: O(1) auxiliary space.
  std::string validIPAddress(std::string_view query_ip) {
    // Checking exact delimiter counts first guarantees the split produces
    // strictly 4 IPv4 parts or 8 IPv6 parts, bounding auxiliary memory to O(1).
    if (std::ranges::count(query_ip, '.') == 3) {
      const auto parts = split_segments(query_ip, '.');
      if (std::ranges::all_of(parts, is_valid_ipv4_segment)) {
        return "IPv4";
      }
      return "Neither";
    }

    if (std::ranges::count(query_ip, ':') == 7) {
      const auto parts = split_segments(query_ip, ':');
      if (std::ranges::all_of(parts, is_valid_ipv6_segment)) {
        return "IPv6";
      }
      return "Neither";
    }

    return "Neither";
  }

 private:
  // Splits `s` on every occurrence of `delim`, preserving empty segments
  // (e.g. from consecutive or leading/trailing delimiters).
  static std::vector<std::string_view> split_segments(std::string_view s,
                                                      char delim) {
    std::vector<std::string_view> parts;
    std::size_t start = 0uz;
    while (true) {
      const std::size_t pos = s.find(delim, start);
      if (pos == std::string_view::npos) {
        parts.push_back(s.substr(start));
        break;
      }
      parts.push_back(s.substr(start, pos - start));
      start = pos + 1uz;
    }
    return parts;
  }

  // Returns true iff `c` is an ASCII hexadecimal digit (`0-9`, `a-f`, `A-F`).
  static bool is_hex_digit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
  }

  // Validates a single IPv4 decimal octet: 1..3 ASCII digits, no leading zero
  // on multi-digit numbers, and numeric value in [0, 255].
  static bool is_valid_ipv4_segment(std::string_view segment) {
    if (segment.empty() || segment.size() > 3uz) {
      return false;
    }
    // Reject leading zeros such as "00" or "01", while allowing single "0".
    if (segment.size() > 1uz && segment[0] == '0') {
      return false;
    }

    int value = 0;
    for (const char c : segment) {
      if (c < '0' || c > '9') {
        return false;
      }
      value = (value * 10) + (c - '0');
    }
    return value <= 255;
  }

  // Validates a single IPv6 hexadecimal group: 1..4 ASCII hex digits.
  static bool is_valid_ipv6_segment(std::string_view segment) {
    if (segment.empty() || segment.size() > 4uz) {
      return false;
    }
    return std::ranges::all_of(segment, is_hex_digit);
  }
};

LC_PROBLEM(468, "Validate IP Address", "string", ctx) {
  ctx.test("example_1_valid_ipv4", [&] {
    Solution sol;
    ctx.expect_eq(sol.validIPAddress("172.16.254.1"), "IPv4");
  });

  ctx.test("example_2_valid_ipv6", [&] {
    Solution sol;
    ctx.expect_eq(sol.validIPAddress("2001:0db8:85a3:0:0:8A2E:0370:7334"),
                  "IPv6");
  });

  ctx.test("example_3_ipv4_out_of_range", [&] {
    Solution sol;
    ctx.expect_eq(sol.validIPAddress("256.256.256.256"), "Neither");
  });

  ctx.test("python_build_invalid_hex_char", [&] {
    Solution sol;
    ctx.expect_eq(sol.validIPAddress("20EE:FGb8:85a3:0:0:8A2E:0370:7334"),
                  "Neither");
  });

  ctx.test("ipv4_edge_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.validIPAddress("0.0.0.0"), "IPv4");
    ctx.expect_eq(sol.validIPAddress("255.255.255.255"), "IPv4");
    ctx.expect_eq(sol.validIPAddress("172.16.254.01"), "Neither");
    ctx.expect_eq(sol.validIPAddress("172.16.254.1."), "Neither");
    ctx.expect_eq(sol.validIPAddress("1.2.3."), "Neither");
    ctx.expect_eq(sol.validIPAddress("+1.2.3.4"), "Neither");
    ctx.expect_eq(sol.validIPAddress("-1.2.3.4"), "Neither");
  });

  ctx.test("ipv6_edge_cases", [&] {
    Solution sol;
    ctx.expect_eq(sol.validIPAddress("2001:0db8:85a3:0000:0000:8a2e:0370:7334"),
                  "IPv6");
    ctx.expect_eq(sol.validIPAddress("2001:0db8:85a3::8A2E:0370:7334"),
                  "Neither");
    ctx.expect_eq(
        sol.validIPAddress("02001:0db8:85a3:0000:0000:8a2e:0370:7334"),
        "Neither");
    ctx.expect_eq(sol.validIPAddress("2001:0db8:85a3:0:0:+1:0370:7334"),
                  "Neither");
    ctx.expect_eq(sol.validIPAddress("2001:0db8:85a3:0:0:0x1:0370:7334"),
                  "Neither");
    ctx.expect_eq(sol.validIPAddress(""), "Neither");
  });
}

}  // namespace
