// LeetCode 811: Subdomain Visit Count
// https://leetcode.com/problems/subdomain-visit-count/
//
// Problem:
//   A count-paired domain has the format `"rep d1.d2.d3"` or `"rep d1.d2"`,
//   meaning the domain was visited `rep` times. Visiting a domain also
//   implicitly visits every parent subdomain (e.g., visiting
//   `"discuss.leetcode.com"` counts toward `"discuss.leetcode.com"`,
//   `"leetcode.com"`, and `"com"`). Given an array `cpdomains` of count-paired
//   domains, return the total visit count for every subdomain in `"total dom"`
//   format (in any order).
//   Constraints: 1 <= cpdomains.length <= 100, 1 <= cpdomains[i].length <= 100,
//   1 <= rep <= 10^4, lowercase English letters and dots.
//
// Algorithm:
//   1. Maintain a hash map `counts` keyed by `std::string_view` pointing
//      directly into the suffixes of each input entry (zero string copies
//      during aggregation).
//   2. For each `"rep domain"` string:
//        - Split on the single space `' '` and parse `rep` via
//          `std::from_chars`.
//        - Repeatedly add `rep` to `counts[domain]` and strip the leftmost
//          label up to and including the next `'.'` until no dot remains.
//   3. Format each `(subdomain, total)` entry in `counts` as
//      `"<total> <subdomain>"` and return the resulting vector.
//   Time Complexity: O(N * L), where N = cpdomains.size() and L = max entry
//   length (at most 3 subdomain levels per entry).
//   Space Complexity: O(N * L) for the aggregated counts and output strings.
//
// Data Structures:
//   - Hash map (`std::unordered_map<std::string_view, int>`): aggregates total
//     visits per unique subdomain suffix in O(1) average time per level.
//
// Real-World Applications:
//   - Web analytics, CDN, and DNS telemetry roll-ups aggregating traffic
//     hierarchically from leaf hostnames up to parent domains and TLDs.
//   - Hierarchical metric aggregation in observability systems (e.g., summing
//     stats by `service.cluster.region` namespaces).
//   - File-system and object-storage usage attribution rolled up across parent
//     directory prefixes.
//
// Related LeetCode Problems:
//   - #1233 Remove Sub-Folders from the Filesystem [done] (hierarchical path prefix handling)
//   - #609 Find Duplicate File in System [done] (parsing space-delimited paths into hash map)
//   - #692 Top K Frequent Words [done] (string frequency counting via hash map)
//   - #208 Implement Trie [done] (Prefix Tree) (alternative prefix/suffix tree aggregation)

#include "framework/lc_framework.h"

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Aggregates visit counts across all subdomains of `cpdomains` and formats
  // each result as "<count> <subdomain>".
  // Time: O(N * L), Space: O(N * L).
  std::vector<std::string> subdomainVisits(
      const std::vector<std::string>& cpdomains) {
    // Non-owning string_view keys slice directly into `cpdomains` entries,
    // which remain valid for the entire function call.
    std::unordered_map<std::string_view, int> counts;
    counts.reserve(cpdomains.size() * 3uz);

    for (const std::string& entry : cpdomains) {
      const std::string_view view(entry);
      const std::size_t space_pos = view.find(' ');
      if (space_pos == std::string_view::npos) {
        continue;
      }

      // Parse the integer visit count preceding the space.
      int visits = 0;
      const std::string_view count_part = view.substr(0uz, space_pos);
      const auto parse_res = std::from_chars(
          count_part.data(), count_part.data() + count_part.size(), visits);
      if (parse_res.ec != std::errc{}) {
        continue;
      }

      // Walk from the full domain up through every parent subdomain suffix.
      std::string_view domain = view.substr(space_pos + 1uz);
      while (true) {
        counts[domain] += visits;
        const std::size_t dot_pos = domain.find('.');
        if (dot_pos == std::string_view::npos) {
          break;
        }
        domain.remove_prefix(dot_pos + 1uz);
      }
    }

    // Materialize the aggregated "<total> <subdomain>" strings.
    std::vector<std::string> result;
    result.reserve(counts.size());
    for (const auto& [subdomain, total] : counts) {
      std::string line = std::to_string(total);
      line.push_back(' ');
      line.append(subdomain);
      result.push_back(std::move(line));
    }

    return result;
  }
};

// Sorts a vector of strings in place so order-independent outputs compare
// deterministically in tests.
std::vector<std::string> sortedCopy(std::vector<std::string> items) {
  std::ranges::sort(items);
  return items;
}

LC_PROBLEM(811, "Subdomain Visit Count", "hash", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1", [&] {
    const std::vector<std::string> input = {"9001 discuss.leetcode.com"};
    const std::vector<std::string> expected = {
        "9001 com", "9001 discuss.leetcode.com", "9001 leetcode.com"};
    ctx.expect_eq(sortedCopy(sol.subdomainVisits(input)), expected);
  });

  ctx.test("leetcode_example_2_from_build", [&] {
    const std::vector<std::string> input = {
        "900 google.mail.com", "50 yahoo.com", "1 intel.mail.com",
        "5 wiki.org"};
    const std::vector<std::string> expected = {
        "1 intel.mail.com",    "5 org",        "5 wiki.org", "50 yahoo.com",
        "900 google.mail.com", "901 mail.com", "951 com"};
    ctx.expect_eq(sortedCopy(sol.subdomainVisits(input)), expected);
  });

  ctx.test("duplicate_exact_domains_accumulate", [&] {
    const std::vector<std::string> input = {"10 a.b.com", "20 a.b.com"};
    const std::vector<std::string> expected = {"30 a.b.com", "30 b.com",
                                               "30 com"};
    ctx.expect_eq(sortedCopy(sol.subdomainVisits(input)), expected);
  });

  ctx.test("two_level_domain_only", [&] {
    const std::vector<std::string> input = {"42 example.io"};
    const std::vector<std::string> expected = {"42 example.io", "42 io"};
    ctx.expect_eq(sortedCopy(sol.subdomainVisits(input)), expected);
  });
}

}  // namespace
