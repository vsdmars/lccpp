// LeetCode 535: Encode And Decode Tinyurl
// https://leetcode.com/problems/encode-and-decode-tinyurl/
//
// Problem:
//   Design a TinyURL URL-shortening service with two operations:
//     - encode(longUrl): returns a shortened URL (e.g.
//       "http://tinyurl.com/4e9iAk") for the given original URL.
//     - decode(shortUrl): returns the exact original long URL corresponding to
//       the given shortened URL.
//   Repeatedly encoding the same long URL should remain consistent, and
//   distinct long URLs must map to distinct short URLs without collisions.
//
// Examples:
//   Example 1:
//     Input:  encode("https://leetcode.com/problems/design-tinyurl"), decode(tiny)
//     Output: tiny = "http://tinyurl.com/a1B2c3" (6-char Base-62 code),
//             decode(tiny) = "https://leetcode.com/problems/design-tinyurl"
//     Explanation:
//       - encode checks url_to_code_ for
//         "https://leetcode.com/problems/design-tinyurl" and does not find an
//         existing entry.
//       - generateCode draws 6 uniform random characters from the 62-character
//         kAlphabet (for example "a1B2c3") and verifies code_to_url_ does not
//         already contain it.
//       - Both maps record the bijection ("a1B2c3" <->
//         "https://leetcode.com/problems/design-tinyurl") and buildShortUrl
//         prepends "http://tinyurl.com/" to return "http://tinyurl.com/a1B2c3".
//       - decode verifies the "http://tinyurl.com/" prefix, strips it to extract
//         "a1B2c3", and returns the stored original URL from code_to_url_.
//
//   Example 2:
//     Input:  encode("http://haha"), encode("http://haha"),
//             encode("https://example.org/path?q=1&b=2#frag"), decode(tiny1_a),
//             decode(tiny2)
//     Output: tiny1_a == tiny1_b != tiny2; decode(tiny1_a) = "http://haha",
//             decode(tiny2) = "https://example.org/path?q=1&b=2#frag"
//     Explanation:
//       - The first encode("http://haha") generates an unused 6-character code c1,
//         stores ("http://haha" <-> c1) in both maps, and returns tiny1_a =
//         "http://tinyurl.com/" + c1.
//       - The second encode("http://haha") hits url_to_code_["http://haha"] and
//         immediately returns buildShortUrl(c1), guaranteeing tiny1_a == tiny1_b.
//       - encode("https://example.org/path?q=1&b=2#frag") generates a new code c2
//         not present in code_to_url_ (so c2 != c1 and tiny2 != tiny1_a) and
//         records the second bijection.
//       - decode(tiny1_a) and decode(tiny2) look up c1 and c2 in code_to_url_ to
//         recover "http://haha" and "https://example.org/path?q=1&b=2#frag"
//         respectively.
//
//   Example 3:
//     Input:  decode("pica"), decode("http://tinyurl.com/000000")
//     Output: "", ""
//     Explanation:
//       - For "pica", short_url.starts_with("http://tinyurl.com/") is false, so
//         decode returns an empty string immediately.
//       - For "http://tinyurl.com/000000", decode strips the prefix to get code
//         "000000", fails to find it in code_to_url_, and returns an empty string.
//
// Algorithm:
//   Maintain a bidirectional hash-map bijection between original URLs and
//   randomly generated 6-character Base-62 codes (62^6 ≈ 56.8 billion unique
//   codes):
//     - encode(longUrl): if `longUrl` already exists in `url_to_code_`, return
//       its existing shortened URL immediately (idempotency). Otherwise, draw
//       6 random characters from [a-zA-Z0-9] using a PRNG until a code not
//       present in `code_to_url_` is found, record both directions, and return
//       `kBaseUrl + code`.
//     - decode(shortUrl): verify the `kBaseUrl` prefix, extract the trailing
//       code, and look up the original URL in `code_to_url_` (returning an
//       empty string if unknown).
//   Time Complexity:  O(1) expected time per encode() and decode() (collision
//                     probability is negligible for << 62^6 stored URLs).
//   Space Complexity: O(U * L) to store U distinct URLs of average length L.
//
// Data Structures:
//   - std::unordered_map<std::string, std::string> url_to_code_: deduplicates
//     repeated encode() requests for the same long URL.
//   - std::unordered_map<std::string, std::string> code_to_url_: resolves
//     short codes back to original URLs in O(1) expected time and guards
//     against random code collisions.
//   - std::mt19937_64 rng_: 64-bit Mersenne Twister generator for uniform
//     Base-62 short-code generation.
//
// Real-World Applications:
//   - Production URL shorteners (bit.ly, tinyurl.com, t.co) mapping arbitrary
//     URLs to compact opaque tokens stored in distributed key-value stores.
//   - Shareable deep links, QR-code payload compaction, and SMS link tracking.
//   - Opaque capability tokens and session/invite code generation where
//     sequential IDs must not be trivially enumerable.
//
// Related LeetCode Problems:
//   - #146 LRU Cache [done] (hash-map-backed design problem with O(1) lookup)
//   - #380 Insert Delete GetRandom O [done] (1) (randomized O(1) data structure design)
//   - #271 Encode and Decode Strings [done] (reversible string serialization design)
//   - #706 Design HashMap [done] (underlying associative storage mechanics)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>

namespace {

class Solution {
 public:
  // Initializes the codec with non-deterministic entropy from random_device.
  Solution() : rng_(std::random_device{}()) {}

  // Initializes the codec with a caller-supplied PRNG seed.
  explicit Solution(std::uint64_t seed) : rng_(seed) {}

  // Encodes a long URL into a fixed-length Base-62 TinyURL.
  // Calling encode() repeatedly on the same URL returns the identical code.
  // Time: O(L) expected (L = long_url.size()), Space: O(L) for a new URL.
  std::string encode(const std::string& long_url) {
    if (const auto it = url_to_code_.find(long_url); it != url_to_code_.end()) {
      return buildShortUrl(it->second);
    }

    // Retry random code generation until an unused 6-char key is found.
    std::string code = generateCode();
    while (code_to_url_.contains(code)) {
      code = generateCode();
    }

    code_to_url_.emplace(code, long_url);
    url_to_code_.emplace(long_url, code);
    return buildShortUrl(code);
  }

  // Decodes a previously generated TinyURL back to its original long URL.
  // Returns an empty string if the short URL is malformed or unrecognized.
  // Time: O(1) expected, Space: O(1) auxiliary.
  [[nodiscard]] std::string decode(std::string_view short_url) const {
    if (!short_url.starts_with(kBaseUrl)) {
      return {};
    }
    const std::string code(short_url.substr(kBaseUrl.size()));
    if (const auto it = code_to_url_.find(code); it != code_to_url_.end()) {
      return it->second;
    }
    return {};
  }

 private:
  static constexpr std::string_view kBaseUrl = "http://tinyurl.com/";
  static constexpr std::string_view kAlphabet =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  static constexpr std::size_t kCodeLength = 6uz;

  // Concatenates the fixed TinyURL prefix with a generated short code.
  static std::string buildShortUrl(std::string_view code) {
    std::string result;
    result.reserve(kBaseUrl.size() + code.size());
    result.append(kBaseUrl);
    result.append(code);
    return result;
  }

  // Draws `kCodeLength` uniform random Base-62 characters.
  std::string generateCode() {
    std::uniform_int_distribution<std::size_t> dist(0uz,
                                                    kAlphabet.size() - 1uz);
    std::string code;
    code.reserve(kCodeLength);
    for (std::size_t i = 0uz; i < kCodeLength; ++i) {
      code.push_back(kAlphabet[dist(rng_)]);
    }
    return code;
  }

  std::unordered_map<std::string, std::string> url_to_code_;
  std::unordered_map<std::string, std::string> code_to_url_;
  std::mt19937_64 rng_;
};

LC_PROBLEM(535, "Encode And Decode Tinyurl", "hash", ctx) {
  ctx.test("roundtrip_leetcode_example", [&] {
    Solution codec;
    const std::string url = "https://leetcode.com/problems/design-tinyurl";
    const std::string tiny = codec.encode(url);
    ctx.expect_true(tiny.starts_with("http://tinyurl.com/"));
    ctx.expect_eq(tiny.size(), std::string("http://tinyurl.com/").size() + 6uz);
    ctx.expect_eq(codec.decode(tiny), url);
  });

  ctx.test("idempotent_reencode_and_distinct_urls", [&] {
    Solution codec;
    const std::string url1 = "http://haha";
    const std::string url2 = "https://example.org/path?q=1&b=2#frag";
    const std::string tiny1_a = codec.encode(url1);
    const std::string tiny1_b = codec.encode(url1);
    const std::string tiny2 = codec.encode(url2);

    ctx.expect_eq(tiny1_a, tiny1_b);
    ctx.expect_ne(tiny1_a, tiny2);
    ctx.expect_eq(codec.decode(tiny1_a), url1);
    ctx.expect_eq(codec.decode(tiny2), url2);
  });

  ctx.test("unknown_or_malformed_short_url", [&] {
    Solution codec;
    ctx.expect_eq(codec.decode("pica"), std::string(""));
    ctx.expect_eq(codec.decode("http://tinyurl.com/000000"), std::string(""));
  });

  ctx.test("empty_url_and_many_entries", [&] {
    Solution codec(42ULL);
    const std::string empty_tiny = codec.encode("");
    ctx.expect_eq(codec.decode(empty_tiny), std::string(""));

    std::unordered_map<std::string, std::string> expected;
    for (int i = 0; i < 100; ++i) {
      std::string long_url = "https://site.test/item/" + std::to_string(i);
      std::string short_url = codec.encode(long_url);
      ctx.expect_false(expected.contains(short_url));
      expected.emplace(short_url, long_url);
    }
    for (const auto& [short_url, long_url] : expected) {
      ctx.expect_eq(codec.decode(short_url), long_url);
    }
  });
}

}  // namespace
