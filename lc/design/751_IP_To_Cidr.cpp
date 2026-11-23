// LeetCode 751: IP To Cidr
// https://leetcode.com/problems/ip-to-cidr/
//
// Problem:
//   Given a starting IPv4 address string `ip` (four dot-separated decimal
//   octets representing a 32-bit unsigned integer) and an integer `n`, return
//   the shortest list of CIDR blocks (`"a.b.c.d/k"`) that covers the exact
//   inclusive IP range `[ip, ip + n - 1]` with no addresses outside the range.
//
// Algorithm:
//   Convert the starting IPv4 string into a 64-bit unsigned integer `cur`
//   (using 64 bits safely handles `0.0.0.0` with a full 32-bit block and
//   prevents 32-bit wraparound past `255.255.255.255`). Greedily cover as many
//   addresses as possible starting at `cur` on each iteration while `remaining
//   > 0`:
//     1. Alignment constraint: a CIDR block of size 2^e starting at `cur` is
//        valid iff `cur` is a multiple of 2^e, i.e., `e <= countr_zero(cur)`
//        (or `e <= 32` when `cur == 0`).
//     2. Range constraint: the block cannot exceed `remaining` addresses, so
//        `2^e <= remaining`, i.e., `e <= floor(log2(remaining)) =
//        bit_width(remaining) - 1`.
//     3. Pick the maximum valid exponent `e = min(trailing_zeros, max_exp)`,
//        emit `formatIp(cur) + "/" + (32 - e)`, advance `cur += (1ULL << e)`,
//        and decrease `remaining -= (1ULL << e)`.
//   Because every greedy step either eliminates the lowest set bit of `cur`
//   (strictly increasing alignment) or halves `remaining`, it is provably
//   optimal and emits at most O(log n) blocks.
//   Time Complexity:  O(log n) — at most 2 * log2(n) <= 64 CIDR blocks, each
//                     computed in O(1) via C++23 `<bit>` operations.
//   Space Complexity: O(log n) for the returned list of CIDR strings.
//
// Data Structures:
//   - 64-bit unsigned integer bitmask representation of IPv4 addresses.
//   - `std::vector<std::string>` storing the resulting minimal CIDR cover.
//
// Real-World Applications:
//   - Firewall ACL, VPC security group, and BGP routing-table rule compaction
//     (minimizing TCAM entries for contiguous IP ranges).
//   - Cloud subnet allocation and IPAM (IP Address Management) range splitting.
//   - CDN/WAF allowlist and blocklist summarization.
//
// Related LeetCode Problems:
//   - #468 Validate IP Address [done] (IPv4/IPv6 parsing and validation)
//   - #93 Restore IP Addresses [done] (IPv4 dotted-quad octet construction)
//   - #191 Number of 1 Bits [done] (bit-level manipulation of 32-bit integers)
//   - #201 Bitwise AND of Numbers Range [new] (common binary prefix of a range)

#include "framework/lc_framework.h"

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr int kIpv4Bits = 32;

class Solution {
 public:
  // Returns the minimal list of CIDR blocks covering [ip, ip + n - 1] exactly.
  // Time: O(log n), Space: O(log n).
  std::vector<std::string> ipToCIDR(std::string_view ip, int n) {
    std::uint64_t cur = parseIp(ip);
    auto remaining = static_cast<std::uint64_t>(n);
    std::vector<std::string> result;

    while (remaining > 0U) {
      // Alignment limit: number of trailing zero bits in `cur` determines the
      // largest aligned power-of-two block starting at `cur`. Special-case
      // 0.0.0.0 which is aligned to 2^32.
      const int trailing_zeros =
          (cur == 0U) ? kIpv4Bits : std::countr_zero(cur);

      // Budget limit: largest power of two that does not overshoot `remaining`.
      const int max_budget_exp = std::bit_width(remaining) - 1;

      // Choose the largest valid block size 2^exp satisfying both limits.
      const int exp = std::min(trailing_zeros, max_budget_exp);
      const std::uint64_t block_size =
          std::uint64_t{1} << static_cast<std::uint32_t>(exp);
      const int prefix_len = kIpv4Bits - exp;

      result.push_back(formatCidr(cur, prefix_len));
      cur += block_size;
      remaining -= block_size;
    }

    return result;
  }

 private:
  // Parses a dotted-quad IPv4 string ("a.b.c.d") into a 64-bit integer.
  static std::uint64_t parseIp(std::string_view ip) {
    std::uint64_t value = 0U;
    std::uint64_t octet = 0U;

    for (const char ch : ip) {
      if (ch == '.') {
        value = (value << 8U) | octet;
        octet = 0U;
      } else {
        octet = (octet * 10U) + static_cast<std::uint64_t>(ch - '0');
      }
    }
    return (value << 8U) | octet;
  }

  // Formats an IPv4 integer and prefix length into canonical "a.b.c.d/k" form.
  static std::string formatCidr(std::uint64_t ip, int prefix_len) {
    const auto o1 = static_cast<std::uint32_t>((ip >> 24U) & 0xFFU);
    const auto o2 = static_cast<std::uint32_t>((ip >> 16U) & 0xFFU);
    const auto o3 = static_cast<std::uint32_t>((ip >> 8U) & 0xFFU);
    const auto o4 = static_cast<std::uint32_t>(ip & 0xFFU);

    std::string out;
    out.reserve(18uz);
    out.append(std::to_string(o1));
    out.push_back('.');
    out.append(std::to_string(o2));
    out.push_back('.');
    out.append(std::to_string(o3));
    out.push_back('.');
    out.append(std::to_string(o4));
    out.push_back('/');
    out.append(std::to_string(prefix_len));
    return out;
  }
};

LC_PROBLEM(751, "IP To Cidr", "design", ctx) {
  ctx.test("example_1", [&] {
    Solution sol;
    const std::vector<std::string> expected = {
        "255.0.0.7/32",
        "255.0.0.8/29",
        "255.0.0.16/32",
    };
    ctx.expect_eq(sol.ipToCIDR("255.0.0.7", 10), expected);
  });

  ctx.test("example_2", [&] {
    Solution sol;
    const std::vector<std::string> expected = {
        "117.145.102.62/31",
        "117.145.102.64/30",
        "117.145.102.68/31",
    };
    ctx.expect_eq(sol.ipToCIDR("117.145.102.62", 8), expected);
  });

  ctx.test("zero_ip_edge_case", [&] {
    Solution sol;
    const std::vector<std::string> expected = {"0.0.0.0/31"};
    ctx.expect_eq(sol.ipToCIDR("0.0.0.0", 2), expected);
  });

  ctx.test("single_address_and_max_ip", [&] {
    Solution sol;
    const std::vector<std::string> expected_max = {"255.255.255.255/32"};
    ctx.expect_eq(sol.ipToCIDR("255.255.255.255", 1), expected_max);

    const std::vector<std::string> expected_zero = {"0.0.0.0/32"};
    ctx.expect_eq(sol.ipToCIDR("0.0.0.0", 1), expected_zero);
  });

  ctx.test("exact_power_of_two_aligned_block", [&] {
    Solution sol;
    const std::vector<std::string> expected = {"192.168.1.0/24"};
    ctx.expect_eq(sol.ipToCIDR("192.168.1.0", 256), expected);
  });
}

}  // namespace
