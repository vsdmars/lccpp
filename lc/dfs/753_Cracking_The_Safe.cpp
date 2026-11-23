// LeetCode 753: Cracking The Safe
// https://leetcode.com/problems/cracking-the-safe/
//
// Problem:
//   A safe is unlocked by an `n`-digit password where every digit lies in
//   `[0, k - 1]`. As digits are typed, the safe continuously checks the last
//   `n` entered digits against the secret password. Return any string of
//   minimum possible length that contains every one of the `k^n` possible
//   `n`-digit passwords as a contiguous substring.
//   Constraints: 1 <= n <= 4, 1 <= k <= 10, 1 <= k^n <= 4096.
//
// Algorithm:
//   The minimum-length string containing all `k^n` length-`n` words over an
//   alphabet of size `k` is a linear De Bruijn sequence of length `k^n + n - 1`
//   (each new digit after the first `n - 1` digits forms a distinct `n`-digit
//   window). Equivalently, it corresponds to a Hamiltonian path on the
//   order-`n` De Bruijn graph (or an Eulerian circuit on the order-`(n - 1)`
//   De Bruijn graph).
//   We construct the sequence using Martin's greedy algorithm:
//     1. Start with a prefix of `n - 1` zeros (`"00...0"`).
//     2. Repeat `k^n` times: take the last `n - 1` digits of the current
//        answer, and try appending digits from `k - 1` down to `0` (largest
//        digit first). Pick the first digit that forms an unvisited length-`n`
//        password, mark it visited, and append that digit to the answer.
//   By Martin's theorem (1934), trying digits in strictly descending order
//   starting from all zeros never gets trapped prematurely and is guaranteed
//   to emit all `k^n` passwords in exact minimum length `k^n + n - 1`.
//   Time Complexity: O(n * k * k^n) to test candidate transitions and hash
//     length-`n` strings.
//   Space Complexity: O(n * k^n) to store all visited length-`n` passwords.
//
// Data Structures:
//   - std::string `ans`: accumulates the output De Bruijn sequence.
//   - std::unordered_set<std::string> `visited`: tracks which of the `k^n`
//     length-`n` passwords have already appeared as a substring.
//
// Real-World Applications:
//   - De Bruijn graph genome assembly in bioinformatics (reconstructing DNA
//     sequences from overlapping k-mers).
//   - Optical rotary encoders and absolute position sensors (encoding angular
//     position with minimal track length).
//   - Hardware bit-scan / trailing-zero lookup tables (De Bruijn multiplication
//     trick for fast bit indexing).
//
// Related LeetCode Problems:
//   - #332 Reconstruct Itinerary [done] (Hierholzer's Eulerian path DFS on a directed graph)
//   - #2097 Valid Arrangement of Pairs [new] (Eulerian path reconstruction over directed edges)
//   - #943 Find the Shortest Superstring [new] (general shortest common superstring via bitmask DP)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_set>

namespace {

class Solution {
 public:
  // Constructs a minimum-length De Bruijn sequence covering all `k^n`
  // length-`n` combinations over digits `0..k-1`.
  // Time: O(n * k * k^n); Space: O(n * k^n).
  std::string crackSafe(int n, int k) {
    // Compute total number of distinct length-`n` passwords: k^n.
    std::size_t total_passwords = 1uz;
    for (int i = 0; i < n; ++i) {
      total_passwords *= static_cast<std::size_t>(k);
    }

    const auto prefix_len = static_cast<std::size_t>(n - 1);
    std::string ans(prefix_len, '0');
    ans.reserve(total_passwords + prefix_len);

    std::unordered_set<std::string> visited;
    visited.reserve(total_passwords);

    // Reusable length-`n` buffer holding `<last n-1 digits><trial digit>`.
    std::string candidate(static_cast<std::size_t>(n), '0');

    for (std::size_t step = 0uz; step < total_passwords; ++step) {
      if (prefix_len > 0uz) {
        candidate.replace(0uz, prefix_len, ans, ans.size() - prefix_len,
                          prefix_len);
      }

      // Martin's algorithm: pick the largest digit yielding an unseen window
      // so the all-zero state is deferred until the very last transition.
      for (int digit = k - 1; digit >= 0; --digit) {
        const auto ch = static_cast<char>('0' + digit);
        candidate.back() = ch;
        if (visited.insert(candidate).second) {
          ans.push_back(ch);
          break;
        }
      }
    }

    return ans;
  }
};

// Verifies that `seq` is a valid minimum-length De Bruijn sequence for `(n, k)`:
// exact length `k^n + n - 1`, valid alphabet `0..k-1`, and every length-`n`
// window is unique (hence all `k^n` combinations are present).
bool is_valid_de_bruijn(std::string_view seq, int n, int k) {
  std::size_t total_passwords = 1uz;
  for (int i = 0; i < n; ++i) {
    total_passwords *= static_cast<std::size_t>(k);
  }
  const std::size_t expected_len =
      total_passwords + static_cast<std::size_t>(n - 1);
  if (seq.size() != expected_len) {
    return false;
  }

  const bool valid_alphabet = std::ranges::all_of(seq, [k](char ch) {
    return ch >= '0' && ch < static_cast<char>('0' + k);
  });
  if (!valid_alphabet) {
    return false;
  }

  std::unordered_set<std::string_view> seen;
  seen.reserve(total_passwords);
  const auto window = static_cast<std::size_t>(n);
  for (std::size_t i = 0uz; i < total_passwords; ++i) {
    seen.insert(seq.substr(i, window));
  }
  return seen.size() == total_passwords;
}

LC_PROBLEM(753, "Cracking The Safe", "dfs", ctx) {
  ctx.test("example_1_n1_k2", [&] {
    Solution sol;
    const std::string res = sol.crackSafe(1, 2);
    ctx.expect_true(is_valid_de_bruijn(res, 1, 2));
  });

  ctx.test("example_2_n2_k2", [&] {
    Solution sol;
    const std::string res = sol.crackSafe(2, 2);
    ctx.expect_eq(res, "01100");
    ctx.expect_true(is_valid_de_bruijn(res, 2, 2));
  });

  ctx.test("single_digit_single_symbol", [&] {
    Solution sol;
    const std::string res = sol.crackSafe(1, 1);
    ctx.expect_eq(res, "0");
    ctx.expect_true(is_valid_de_bruijn(res, 1, 1));
  });

  ctx.test("n3_k2_binary_triples", [&] {
    Solution sol;
    const std::string res = sol.crackSafe(3, 2);
    ctx.expect_true(is_valid_de_bruijn(res, 3, 2));
  });

  ctx.test("n3_k4_larger_alphabet", [&] {
    Solution sol;
    const std::string res = sol.crackSafe(3, 4);
    ctx.expect_true(is_valid_de_bruijn(res, 3, 4));
  });
}

}  // namespace
