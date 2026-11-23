// LeetCode 204: Count Primes
// https://leetcode.com/problems/count-primes/
//
// Problem:
//   Given an integer `n` (0 <= n <= 5 * 10^6), return the number of prime
//   numbers that are strictly less than `n`.
//
// Examples:
//   Example 1:
//     Input:  n = 10
//     Output: 4
//     Explanation:
//       - Allocate is_prime of size 10 initialized to 1, and set is_prime[0] = 0
//         and is_prime[1] = 0.
//       - At p = 2 (2 * 2 = 4 < 10), is_prime[2] == 1, so cross off multiples 4, 6,
//         and 8 by setting them to 0.
//       - At p = 3 (3 * 3 = 9 < 10), is_prime[3] == 1, so set is_prime[9] = 0; the
//         outer loop terminates before p = 4 since 4 * 4 = 16 >= 10.
//       - Count remaining 1 entries in is_prime (at indices 2, 3, 5, and 7) via
//         std::ranges::count and return 4.
//
//   Example 2:
//     Input:  n = 2
//     Output: 0
//     Explanation:
//       - Because n <= 2, the initial guard if (n <= 2) returns 0 immediately
//         without allocating the sieve array.
//       - No prime numbers exist strictly less than 2.
//
//   Example 3:
//     Input:  n = 4
//     Output: 2
//     Explanation:
//       - Allocate is_prime of size 4 initialized to 1 and mark is_prime[0] = 0 and
//         is_prime[1] = 0.
//       - Skip the sieving loop because at p = 2, p * p = 4 is not strictly less
//         than limit = 4.
//       - Count remaining 1 entries in is_prime (indices 2 and 3) and return 2.
//
// Algorithm:
//   Sieve of Eratosthenes.
//   Maintain a byte array `is_prime[0..n-1]` initialized to 1 (candidate prime)
//   with `is_prime[0] = is_prime[1] = 0`. Iterate `p` from 2 while `p * p < n`.
//   Whenever `is_prime[p]` is still 1, `p` is prime, so mark every multiple
//   `p * p, p * p + p, ...` below `n` as composite (0). Starting at `p * p` is
//   valid because any smaller multiple `k * p` with `k < p` already has a prime
//   factor `<= k < p` and was crossed off in an earlier pass. Finally, count all
//   remaining entries equal to 1.
//   Time Complexity:  O(n log log n)
//   Space Complexity: O(n)
//
// Data Structures:
//   - std::vector<std::uint8_t>: contiguous byte sieve storing primality flags
//     for `[0, n)`. Using `std::uint8_t` instead of `std::vector<bool>` avoids
//     bit-masking overhead during tight inner-loop writes.
//
// Real-World Applications:
//   - Cryptography key generation (RSA/Diffie-Hellman prime candidates are
//     pre-filtered with small-prime sieves before Miller-Rabin primality tests).
//   - Hash-table sizing and prime-modulus selection in bloom filters, rolling
//     hashes, and distributed consistent-hashing rings.
//   - Number-theoretic libraries and segmented prime sieves used in scientific
//     computing and integer factorization pipelines.
//
// Related LeetCode Problems:
//   - #263 Ugly Number [new] (checks whether a number has only prime factors 2, 3, 5)
//   - #264 Ugly Number II [new] (generates numbers built from small prime factors)
//   - #279 Perfect Squares [done] (number-theoretic decomposition up to n)
//   - #2523 Closest Prime Numbers in Range [new] (direct application of Sieve of Eratosthenes)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Counts prime numbers strictly less than `n` using the Sieve of
  // Eratosthenes.
  //
  // Invariant: after processing candidate `p`, every composite number `< n`
  // with smallest prime factor `<= p` has been marked 0 in `is_prime`.
  //
  // Time: O(n log log n), Space: O(n).
  int countPrimes(int n) {
    if (n <= 2) {
      return 0;
    }

    const auto limit = static_cast<std::size_t>(n);
    // 1 indicates "still assumed prime"; 0 indicates composite (or < 2).
    std::vector<std::uint8_t> is_prime(limit, 1U);
    is_prime[0] = 0U;
    is_prime[1] = 0U;

    // Only primes up to sqrt(n - 1) can have a composite multiple < n that has
    // not already been marked by a smaller prime factor.
    for (std::size_t p = 2uz; (p * p) < limit; ++p) {
      if (is_prime[p] == 0U) {
        continue;
      }

      // Cross off multiples starting from p^2; smaller multiples k*p (k < p)
      // were already crossed off when processing the prime factors of k.
      for (std::size_t multiple = (p * p); multiple < limit; multiple += p) {
        is_prime[multiple] = 0U;
      }
    }

    return static_cast<int>(std::ranges::count(is_prime, std::uint8_t{1U}));
  }
};

LC_PROBLEM(204, "Count Primes", "math", ctx) {
  Solution sol;

  ctx.test("example_10", [&] { ctx.expect_eq(sol.countPrimes(10), 4); });

  ctx.test("small_edge_cases", [&] {
    ctx.expect_eq(sol.countPrimes(0), 0);
    ctx.expect_eq(sol.countPrimes(1), 0);
    ctx.expect_eq(sol.countPrimes(2), 0);
    ctx.expect_eq(sol.countPrimes(3), 1);
    ctx.expect_eq(sol.countPrimes(4), 2);
  });

  ctx.test("known_prime_counting_values", [&] {
    // pi(99) = 25, pi(999) = 168, pi(9999) = 1229.
    ctx.expect_eq(sol.countPrimes(100), 25);
    ctx.expect_eq(sol.countPrimes(1000), 168);
    ctx.expect_eq(sol.countPrimes(10000), 1229);
  });
}

}  // namespace
