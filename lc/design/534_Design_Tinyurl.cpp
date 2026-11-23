// LeetCode 534: Design Tinyurl
// https://leetcode.com/problems/design-tinyurl/
//
// Problem:
//   Design a URL shortening service (like TinyURL) that maps arbitrary long
//   URLs to compact short URLs of the form "http://tinyurl.com/<6-char-code>"
//   where the 6-character identifier uses the Base62 alphabet [0-9a-zA-Z],
//   and resolves short URLs back to their exact original long URLs. No two
//   distinct long URLs may collide on the same short URL, and encoding the
//   same long URL repeatedly should be idempotent.
//
// Examples:
//   Example 1:
//     Input:  TinyUrlService();
//             encode("https://leetcode.com/problems/design-tinyurl");
//             decode("http://tinyurl.com/000001")
//     Output: ["http://tinyurl.com/000001",
//             "https://leetcode.com/problems/design-tinyurl"]
//     Explanation:
//       - encode checks long_to_code_ for the URL, misses, and calls encode_id(1)
//         using next_id_ = 1.
//       - encode_id(1) computes Base62 digits right to left into a 6-character
//         string ("000001") and increments next_id_ to 2.
//       - encode stores both directions in code_to_long_ and long_to_code_ and
//         returns "http://tinyurl.com/000001".
//       - decode verifies the "http://tinyurl.com/" prefix, looks up "000001" in
//         code_to_long_, and returns the original URL.
//
//   Example 2:
//     Input:  TinyUrlService(); s1 =
//             encode("https://example.com/articles/system-design"); s1_dup =
//             encode("https://example.com/articles/system-design"); s2 =
//             encode("https://example.com/articles/distributed-cache"); decode(s1);
//             decode(s2); size()
//     Output: [s1 = "http://tinyurl.com/000001", s1_dup =
//             "http://tinyurl.com/000001", s2 = "http://tinyurl.com/000002",
//             "https://example.com/articles/system-design",
//             "https://example.com/articles/distributed-cache", 2]
//     Explanation:
//       - First encode inserts the system-design URL with next_id_ = 1, mapping
//         code "000001" in both maps and bumping next_id_ to 2.
//       - Second encode with the same URL hits long_to_code_ and idempotently
//         returns "http://tinyurl.com/000001" without incrementing next_id_.
//       - encode on the distributed-cache URL misses long_to_code_, assigns
//         next_id_ = 2 to code "000002", and brings size() to 2.
//       - decode on both short URLs strips the base prefix and retrieves the two
//         distinct original URLs from code_to_long_.
//
//   Example 3:
//     Input:  TinyUrlService(); decode("http://tinyurl.com/ffffff");
//             decode("https://other.domain/000001"); decode("")
//     Output: ["", "", ""]
//     Explanation:
//       - decode("http://tinyurl.com/ffffff") matches kBaseUrl, extracts code
//         "ffffff", misses in code_to_long_, and returns an empty string.
//       - decode("https://other.domain/000001") and decode("") fail
//         short_url.starts_with("http://tinyurl.com/") and return an empty string
//         immediately.
//
// Algorithm:
//   Assign each newly seen long URL a strictly increasing 64-bit sequence ID
//   and bijectively encode that ID into a fixed-width 6-character Base62 code
//   (62^6 = 56,800,235,584 unique codes). Maintain two hash maps:
//     - long_to_code_ for O(1) idempotent deduplication on encode().
//     - code_to_long_ for O(1) lookup on decode().
//   In a distributed deployment (the system-design discussion in #534), the
//   sequence generator is replaced by range-sharded ID allocators or Snowflake
//   IDs, the read-heavy decode path sits behind a Redis/Memcached LRU cache,
//   and the authoritative mapping is sharded across a NoSQL key-value store.
//   Time Complexity:  O(L) per encode() and decode(), where L is the URL
//                     length (Base62 conversion itself is O(1) over 6 digits).
//   Space Complexity: O(N * L) for N distinct stored URLs.
//
// Data Structures:
//   - std::unordered_map<std::string, std::string> (long_to_code_ &
//     code_to_long_): Bidirectional O(1) average lookup tables guaranteeing
//     uniqueness and idempotency.
//   - Monotonic std::uint64_t counter: Collision-free Base62 key generator.
//
// Real-World Applications:
//   - Production URL shorteners (bit.ly, TinyURL, t.co, goo.gl) with HTTP 301
//     / 302 redirection, analytics tracking, and link expiration.
//   - Short opaque share links for cloud documents, paste bins, and deep links
//     in mobile messaging and QR codes.
//   - Distributed compact-ID encoders (YouTube video IDs, Snowflake-to-Base62
//     external object identifiers).
//
// Related LeetCode Problems:
//   - #535 Encode and Decode TinyURL [done] (algorithmic implementation twin)
//   - #146 LRU Cache [done] (read-heavy caching layer for hot short URLs)
//   - #460 LFU Cache [done] (frequency-aware cache for viral short links)
//   - #355 Design Twitter [new] (high-QPS read/write system design)
//   - #362 Design Hit Counter [done] (rate/QPS metrics for shortened URLs)

#include "framework/lc_framework.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace {

// Bijective Base62 URL shortening service supporting fixed 6-character codes.
class TinyUrlService {
 public:
  static constexpr std::string_view kBaseUrl = "http://tinyurl.com/";
  static constexpr std::string_view kAlphabet =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static constexpr std::size_t kCodeLength = 6uz;
  static constexpr std::uint64_t kBase = 62ULL;

  TinyUrlService() = default;

  // Encodes `long_url` into a unique 6-char Base62 short URL. Repeated calls
  // with the same `long_url` return the exact same short URL.
  // Time: O(L) average. Space: O(L) for a newly inserted URL.
  std::string encode(std::string_view long_url) {
    std::string key(long_url);
    const auto existing = long_to_code_.find(key);
    if (existing != long_to_code_.end()) {
      return make_short_url(existing->second);
    }

    // Generate the next collision-free Base62 identifier and persist both
    // lookup directions.
    std::string code = encode_id(next_id_);
    ++next_id_;

    std::string short_url = make_short_url(code);
    code_to_long_.emplace(code, key);
    long_to_code_.emplace(std::move(key), std::move(code));
    return short_url;
  }

  // Decodes `short_url` back to the original long URL, or returns "" if the
  // prefix is invalid or the identifier does not exist.
  // Time: O(L) average. Space: O(L) for the returned string.
  [[nodiscard]] std::string decode(std::string_view short_url) const {
    if (!short_url.starts_with(kBaseUrl)) {
      return {};
    }

    const std::string_view code = short_url.substr(kBaseUrl.size());
    const auto it = code_to_long_.find(std::string(code));
    if (it == code_to_long_.end()) {
      return {};
    }
    return it->second;
  }

  // Returns the number of distinct URLs currently stored.
  [[nodiscard]] std::size_t size() const noexcept {
    return code_to_long_.size();
  }

 private:
  // Converts a monotonic numeric ID to a zero-padded 6-character Base62 code.
  static std::string encode_id(std::uint64_t id) {
    std::string code(kCodeLength, kAlphabet[0]);
    for (std::size_t i = kCodeLength; i > 0uz; --i) {
      code[i - 1uz] = kAlphabet[static_cast<std::size_t>(id % kBase)];
      id /= kBase;
    }
    return code;
  }

  // Concatenates the fixed service domain prefix with a 6-character code.
  static std::string make_short_url(std::string_view code) {
    std::string result;
    result.reserve(kBaseUrl.size() + code.size());
    result.append(kBaseUrl);
    result.append(code);
    return result;
  }

  std::uint64_t next_id_ = 1ULL;
  std::unordered_map<std::string, std::string> long_to_code_;
  std::unordered_map<std::string, std::string> code_to_long_;
};

LC_PROBLEM(534, "Design Tinyurl", "design", ctx) {
  ctx.test("encode_decode_roundtrip", [&] {
    TinyUrlService service;
    const std::string url = "https://leetcode.com/problems/design-tinyurl";
    const std::string short_url = service.encode(url);

    ctx.expect_true(short_url.starts_with(TinyUrlService::kBaseUrl));
    const std::size_t code_len =
        short_url.size() - TinyUrlService::kBaseUrl.size();
    ctx.expect_eq(code_len, TinyUrlService::kCodeLength);
    ctx.expect_eq(service.decode(short_url), url);
  });

  ctx.test("idempotent_and_collision_free", [&] {
    TinyUrlService service;
    const std::string url1 = "https://example.com/articles/system-design";
    const std::string url2 = "https://example.com/articles/distributed-cache";

    const std::string s1_first = service.encode(url1);
    const std::string s1_second = service.encode(url1);
    const std::string s2 = service.encode(url2);

    ctx.expect_eq(s1_first, s1_second);
    ctx.expect_ne(s1_first, s2);
    ctx.expect_eq(service.size(), 2uz);
    ctx.expect_eq(service.decode(s1_first), url1);
    ctx.expect_eq(service.decode(s2), url2);
  });

  ctx.test("base62_rollover_many_urls", [&] {
    TinyUrlService service;
    // Exceed kBase (62) to exercise multi-digit Base62 carry propagation.
    for (int i = 0; i < 75; ++i) {
      const std::string long_url =
          "https://service.io/item/" + std::to_string(i);
      const std::string short_url = service.encode(long_url);
      ctx.expect_eq(short_url.size(),
                    TinyUrlService::kBaseUrl.size() +
                        TinyUrlService::kCodeLength);
      ctx.expect_eq(service.decode(short_url), long_url);
    }
    ctx.expect_eq(service.size(), 75uz);
  });

  ctx.test("invalid_and_unknown_short_urls", [&] {
    TinyUrlService service;
    ctx.expect_eq(service.decode("http://tinyurl.com/ffffff"), std::string());
    ctx.expect_eq(service.decode("https://other.domain/000001"), std::string());
    ctx.expect_eq(service.decode(""), std::string());
  });
}

}  // namespace
