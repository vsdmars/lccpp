// LeetCode 1108: Defanging An IP Address
// https://leetcode.com/problems/defanging-an-ip-address/
//
// Problem:
//   Given a valid IPv4 address string `address`, return its "defanged" version
//   by replacing every period character '.' with "[.]".
//
// Algorithm:
//   Perform a single linear pass over `address`, copying every non-period
//   character directly into a pre-reserved output string and expanding every
//   '.' into the 3-character literal "[.]". Pre-reserving `address.size() + 6`
//   bytes guarantees zero reallocations for any valid IPv4 address (which
//   contains exactly 3 periods).
//   Time Complexity:  O(N), where N = address.size().
//   Space Complexity: O(N) for the returned string; O(1) auxiliary space.
//
// Data Structures:
//   - std::string_view: non-owning read-only view of the input address.
//   - std::string: pre-reserved output buffer populated in one pass.
//
// Real-World Applications:
//   - Security operations (SOC) and threat-intelligence reports that "defang"
//     malicious IP addresses and URLs so chat clients, email readers, and
//     ticketing tools do not auto-link or prefetch indicators of compromise.
//   - Escaping special delimiter characters in serializers, regex builders,
//     and log sanitizers.
//
// Related LeetCode Problems:
//   - #468 Validate IP Address [done] (parsing and validating IPv4/IPv6 strings)
//   - #93 Restore IP Addresses [done] (backtracking valid IPv4 segmentations)
//   - #1678 Goal Parser Interpretation [new] (single-pass token replacement)
//   - #2109 Adding Spaces to a String [new] (single-pass string expansion)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

constexpr std::size_t kIpv4ExtraChars = 6uz;

class Solution {
 public:
  // Replaces every '.' in `address` with "[.]" in a single linear pass.
  //
  // Time Complexity:  O(N)
  // Space Complexity: O(N) for output, O(1) auxiliary
  std::string defangIPaddr(std::string_view address) {
    std::string result;
    // A valid IPv4 address has 3 dots, each adding 2 extra bytes ("." -> "[.]").
    result.reserve(address.size() + kIpv4ExtraChars);

    for (const char ch : address) {
      if (ch == '.') {
        result.append("[.]");
      } else {
        result.push_back(ch);
      }
    }

    return result;
  }
};

LC_PROBLEM(1108, "Defanging An IP Address", "string", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    ctx.expect_eq(sol.defangIPaddr("1.1.1.1"), std::string("1[.]1[.]1[.]1"));
  });

  ctx.test("example_2", [&] {
    Solution sol;
    ctx.expect_eq(sol.defangIPaddr("255.100.50.0"),
                  std::string("255[.]100[.]50[.]0"));
  });

  ctx.test("build_case", [&] {
    Solution sol;
    ctx.expect_eq(sol.defangIPaddr("12.1.1.1"), std::string("12[.]1[.]1[.]1"));
  });

  ctx.test("boundary_addresses", [&] {
    Solution sol;
    ctx.expect_eq(sol.defangIPaddr("0.0.0.0"), std::string("0[.]0[.]0[.]0"));
    ctx.expect_eq(sol.defangIPaddr("255.255.255.255"),
                  std::string("255[.]255[.]255[.]255"));
  });
}

}  // namespace
