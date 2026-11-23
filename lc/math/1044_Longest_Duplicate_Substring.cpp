// LeetCode 1044: Longest Duplicate Substring
// https://leetcode.com/problems/longest-duplicate-substring/
//
// Problem:
//   Given a string `s`, consider all duplicated substrings — contiguous
//   substrings of `s` that occur at least twice (occurrences are allowed to
//   overlap). Return any duplicated substring of maximum possible length, or
//   the empty string `""` if no duplicated substring exists.
//   Constraints: 2 <= s.length <= 3 * 10^4, `s` consists of lowercase English
//   letters.
//
// Examples:
//   Example 1:
//     Input:  s = "banana"
//     Output: "ana"
//     Explanation:
//       - Initialize binary search bounds over duplicate length as lo = 1 and hi =
//         5 for s.size() = 6.
//       - Probe mid = 3; find_duplicate_of_length hashes "ban" at 0, "ana" at 1,
//         "nan" at 2, and "ana" at 3, verifies exact match with prev_start = 1, and
//         updates best_start = 3, best_len = 3, lo = 4.
//       - Probe mid = 4; find_duplicate_of_length scans "bana", "anan", and "nana"
//         with no repeated hash match and returns nullopt, narrowing hi = 3.
//       - Terminate when lo (4) > hi (3) and return s.substr(3, 3) = "ana".
//
//   Example 2:
//     Input:  s = "abcd"
//     Output: ""
//     Explanation:
//       - Initialize binary search bounds to lo = 1 and hi = 3 for s.size() = 4
//         with best_start = 0 and best_len = 0.
//       - Probe mid = 2; find_duplicate_of_length hashes "ab", "bc", and "cd"
//         without any repeat, returning nullopt and setting hi = 1.
//       - Probe mid = 1; find_duplicate_of_length hashes "a", "b", "c", and "d"
//         with all unique keys, returning nullopt and setting hi = 0.
//       - Exit the loop since lo (1) > hi (0) and return s.substr(0, 0) = "".
//
//   Example 3:
//     Input:  s = "aaaaa"
//     Output: "aaaa"
//     Explanation:
//       - Initialize binary search bounds to lo = 1 and hi = 4 for s.size() = 5.
//       - Probe mid = 2 and then mid = 3; both find_duplicate_of_length calls match
//         repeating hashes at index 1, advancing best_len to 3 and lo to 4.
//       - Probe mid = 4; find_duplicate_of_length hashes "aaaa" at start = 0 and
//         start = 1, confirms exact equality, and sets best_start = 1, best_len =
//         4, lo = 5.
//       - Stop when lo (5) > hi (4) and return s.substr(1, 4) = "aaaa".
//
// Algorithm:
//   Binary search on substring length combined with double-hashed Rabin-Karp
//   rolling hashes (plus exact substring verification on hash hits).
//   Monotonicity invariant: if `s` contains a duplicated substring of length L,
//   then it also contains duplicated substrings of every length 1..L-1 (any
//   prefix of the duplicate also repeats). Conversely, if no length-L duplicate
//   exists, no length > L duplicate can exist either. Thus we binary search
//   L in [1, N - 1].
//   For a candidate length L, compute two independent polynomial rolling hashes
//   (mod 10^9 + 7 with base 131, and mod 10^9 + 9 with base 137) and pack both
//   into a single 64-bit key `(h1 << 32) | h2`. As the window slides one
//   character right, remove the leading character multiplied by base^(L-1),
//   shift left by multiplying by `base`, and add the incoming character in O(1)
//   time. Whenever a hash key recurs in the map, verify the candidate
//   `std::string_view` against stored start positions to guarantee zero
//   false-positive hash collisions.
//   Time Complexity:  O(N log N) expected (O(log N) binary-search probes, each
//                     performing an O(N) Rabin-Karp scan).
//   Space Complexity: O(N) for the per-probe hash table of starting indices.
//
// Data Structures:
//   - std::unordered_map<std::uint64_t, std::vector<std::size_t>>: maps each
//     64-bit combined rolling hash to the starting indices of length-L
//     substrings that produced it, enabling O(1) expected duplicate detection
//     with exact collision verification.
//   - std::string_view: zero-copy substring slicing for collision checks.
//
// Real-World Applications:
//   - Data compression (LZ77 / gzip / zstd): locating long repeated byte
//     sequences to replace with compact back-references.
//   - Bioinformatics & genomics: identifying long tandem or interspersed DNA
//     repeats inside chromosome sequences.
//   - Plagiarism detection & code deduplication: finding the longest duplicated
//     token or text spans across documents and repositories.
//
// Related LeetCode Problems:
//   - #1062 Longest Repeating Substring [done] (same problem with smaller constraints, returning only the length)
//   - #187 Repeated DNA Sequences [new] (fixed-length rolling hash duplicate substring search)
//   - #718 Maximum Length of Repeated Subarray [new] (longest common contiguous subarray via DP or binary search + rolling hash)
//   - #28 Find the Index of the First Occurrence in a String [done] (exact substring matching / Rabin-Karp foundation)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Finds any longest duplicated (possibly overlapping) substring of `s`, or
  // returns an empty string if all characters are unique.
  // Time: O(N log N) expected, Space: O(N).
  [[nodiscard]] std::string longestDupSubstring(std::string_view s) const {
    if (s.size() < 2uz) {
      return "";
    }

    // Binary search for the largest length L in [1, s.size() - 1] that has at
    // least one duplicate occurrence.
    std::size_t lo = 1uz;
    std::size_t hi = s.size() - 1uz;
    std::size_t best_start = 0uz;
    std::size_t best_len = 0uz;

    while (lo <= hi) {
      const std::size_t mid = lo + ((hi - lo) / 2uz);
      if (const auto dup_start = find_duplicate_of_length(s, mid);
          dup_start.has_value()) {
        best_start = *dup_start;
        best_len = mid;
        lo = mid + 1uz;
      } else {
        hi = mid - 1uz;
      }
    }

    return std::string(s.substr(best_start, best_len));
  }

 private:
  static constexpr std::uint64_t kBase1 = 131ULL;
  static constexpr std::uint64_t kMod1 = 1'000'000'007ULL;
  static constexpr std::uint64_t kBase2 = 137ULL;
  static constexpr std::uint64_t kMod2 = 1'000'000'009ULL;

  // Encodes a character into a non-zero 64-bit digit so leading characters
  // cannot collapse to zero under polynomial hashing.
  [[nodiscard]] static std::uint64_t char_code(char ch) noexcept {
    return static_cast<std::uint64_t>(static_cast<unsigned char>(ch)) + 1ULL;
  }

  // Packs two 30-bit modular hashes into a single 64-bit map key.
  [[nodiscard]] static std::uint64_t combine_hashes(std::uint64_t h1,
                                                    std::uint64_t h2) noexcept {
    return (h1 << 32U) | h2;
  }

  // Scans `s` with a double rolling hash of window width `len` and returns the
  // starting index of a verified duplicate substring if one exists.
  [[nodiscard]] static std::optional<std::size_t> find_duplicate_of_length(
      std::string_view s, std::size_t len) {
    std::uint64_t h1 = 0ULL;
    std::uint64_t h2 = 0ULL;
    // pow1 = kBase1^(len - 1) mod kMod1, pow2 = kBase2^(len - 1) mod kMod2.
    std::uint64_t pow1 = 1ULL;
    std::uint64_t pow2 = 1ULL;

    for (std::size_t i = 0uz; i < len; ++i) {
      const std::uint64_t val = char_code(s[i]);
      h1 = ((h1 * kBase1) + val) % kMod1;
      h2 = ((h2 * kBase2) + val) % kMod2;
      if ((i + 1uz) < len) {
        pow1 = (pow1 * kBase1) % kMod1;
        pow2 = (pow2 * kBase2) % kMod2;
      }
    }

    std::unordered_map<std::uint64_t, std::vector<std::size_t>> seen;
    seen.reserve((s.size() - len) + 1uz);
    seen[combine_hashes(h1, h2)].push_back(0uz);

    // Slide the window across [start .. start + len - 1] in O(1) per step.
    for (std::size_t start = 1uz; (start + len) <= s.size(); ++start) {
      const std::uint64_t out_val = char_code(s[start - 1uz]);
      const std::uint64_t in_val = char_code(s[(start + len) - 1uz]);

      // Subtract the outgoing leading character's contribution, shift left by
      // multiplying by base, and append the incoming trailing character.
      const std::uint64_t drop1 = (out_val * pow1) % kMod1;
      h1 = (((((h1 + kMod1) - drop1) % kMod1) * kBase1) + in_val) % kMod1;

      const std::uint64_t drop2 = (out_val * pow2) % kMod2;
      h2 = (((((h2 + kMod2) - drop2) % kMod2) * kBase2) + in_val) % kMod2;

      const std::uint64_t key = combine_hashes(h1, h2);
      auto& bucket = seen[key];
      const std::string_view candidate = s.substr(start, len);

      // Confirm exact equality to rule out any hash collision.
      for (const std::size_t prev_start : bucket) {
        if (s.substr(prev_start, len) == candidate) {
          return start;
        }
      }
      bucket.push_back(start);
    }

    return std::nullopt;
  }
};

LC_PROBLEM(1044, "Longest Duplicate Substring", "math", ctx) {
  const Solution sol;

  ctx.test("example_1_banana", [&] {
    ctx.expect_eq(sol.longestDupSubstring("banana"), std::string("ana"));
  });

  ctx.test("example_2_all_unique", [&] {
    ctx.expect_eq(sol.longestDupSubstring("abcd"), std::string(""));
  });

  ctx.test("overlapping_same_character", [&] {
    ctx.expect_eq(sol.longestDupSubstring("aaaaa"), std::string("aaaa"));
  });

  ctx.test("non_overlapping_prefix_suffix", [&] {
    ctx.expect_eq(sol.longestDupSubstring("xyzabcxyz"), std::string("xyz"));
  });

  ctx.test("short_duplicate_in_long_string", [&] {
    // Classic test case where "ma" is the unique longest duplicate of length 2.
    ctx.expect_eq(sol.longestDupSubstring(
                      "nnpxouomcofdjuujloanjimymadkuepightrfodmauhrsy"),
                  std::string("ma"));
  });

  ctx.test("tiny_inputs", [&] {
    ctx.expect_eq(sol.longestDupSubstring(""), std::string(""));
    ctx.expect_eq(sol.longestDupSubstring("z"), std::string(""));
    ctx.expect_eq(sol.longestDupSubstring("zz"), std::string("z"));
  });
}

}  // namespace
