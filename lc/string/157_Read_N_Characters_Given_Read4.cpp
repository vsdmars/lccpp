// LeetCode 157: Read N Characters Given Read4
// https://leetcode.com/problems/read-n-characters-given-read4/
//
// Problem:
//   Given an external block-oriented API `read4(buf4)` that reads up to 4
//   contiguous characters at a time from a file into a 4-byte buffer and
//   returns the actual number of characters read (0..4), implement
//   `read(buf, n)` to read at most `n` characters into `buf` and return the
//   actual number of characters copied. `read` is called once per test case.
//
// Algorithm:
//   Repeatedly invoke `read4` into a fixed 4-byte stack-allocated staging
//   buffer `local_buf`, then copy `min(fetched, n - total)` characters into
//   the caller's destination buffer `buf`. Using a staging buffer guarantees
//   we never write past `buf[n - 1]` when `n` is not a multiple of 4. Terminate
//   early as soon as `total == n` or `read4` returns fewer than 4 characters
//   (indicating EOF).
//   Time Complexity:  O(min(N, F)), where F is the file size.
//   Space Complexity: O(1) using a fixed 4-character stack buffer.
//
// Data Structures:
//   - `std::array<char, 4>` staging buffer bridging 4-byte block reads to an
//     arbitrary caller-requested byte count `n`.
//   - Mock `Reader4` stream class encapsulating the backing file string and
//     current read cursor `pos_`.
//
// Real-World Applications:
//   - Buffered I/O streams (`fread`, `std::ifstream`, `bufio.Reader`) adapting
//     fixed-size disk sectors or OS page reads (e.g., 4 KiB blocks) to
//     arbitrary byte-length user reads.
//   - Network socket framing layers reading fixed-size packets or TLS records
//     into caller-provided buffers without overflowing buffer boundaries.
//   - Flash/EEPROM block drivers and audio/video decoder ring buffers.
//
// Related LeetCode Problems:
//   - #158 Read N Characters Given Read4 II [done] - Call multiple times (persists leftover bytes in staging buffer across calls)
//   - #281 Zigzag Iterator [done] (stateful pull-based stream iterator)
//   - #284 Peeking Iterator [done] (one-element lookahead wrapper over a stream)
//   - #341 Flatten Nested List Iterator [done] (buffered streaming adapter)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr std::size_t kChunkSize = 4uz;

// Mock judge API simulating a sequential file accessed only in 4-byte chunks.
class Reader4 {
 public:
  explicit Reader4(std::string content) : content_(std::move(content)) {}

  // Reads up to 4 characters from the underlying file into `buf4` and returns
  // the actual count read (0..4).
  int read4(std::array<char, kChunkSize>& buf4) {
    std::size_t count = 0uz;
    while (count < kChunkSize && pos_ < content_.size()) {
      buf4[count] = content_[pos_];
      ++count;
      ++pos_;
    }
    return static_cast<int>(count);
  }

 private:
  std::string content_;
  std::size_t pos_ = 0uz;
};

class Solution : public Reader4 {
 public:
  explicit Solution(std::string content) : Reader4(std::move(content)) {}

  // Reads up to `n` characters into `buf` using only `read4`, returning the
  // actual number of characters written.
  // Time: O(min(N, FileSize)), Space: O(1).
  int read(std::vector<char>& buf, int n) {
    if (n <= 0 || buf.empty()) {
      return 0;
    }

    const std::size_t target =
        std::min(static_cast<std::size_t>(n), buf.size());
    std::array<char, kChunkSize> local_buf{};
    std::size_t total = 0uz;

    // Loop invariant: `buf[0..total)` holds the first `total` characters of
    // the file, and `total <= target`.
    while (total < target) {
      const int fetched = read4(local_buf);
      if (fetched == 0) {
        break;
      }

      // Copy only as many bytes as still needed to avoid overflowing `buf`.
      const std::size_t to_copy =
          std::min(static_cast<std::size_t>(fetched), target - total);
      for (std::size_t i = 0uz; i < to_copy; ++i) {
        buf[total + i] = local_buf[i];
      }
      total += to_copy;

      // A short read (< 4) guarantees the underlying file has reached EOF.
      if (std::cmp_less(fetched, kChunkSize)) {
        break;
      }
    }

    return static_cast<int>(total);
  }
};

LC_PROBLEM(157, "Read N Characters Given Read4", "string", ctx) {
  ctx.test("n larger than file length", [&] {
    Solution sol("asdlls");
    std::vector<char> buf(42uz, '\0');
    const int cnt = sol.read(buf, 42);
    ctx.expect_eq(cnt, 6);
    ctx.expect_eq(std::string(buf.begin(), buf.begin() + cnt), "asdlls");
  });

  ctx.test("n smaller than file and not multiple of 4", [&] {
    const std::string data =
        "WRBqHdrOkyIDsdRMwRSYLBfaCYEdgxPlrlNppfkOKcqNnuwSmbUcJISmKtXxvRBJTSFzfM"
        "fdRsfbnvhFSqWQaeCZFKlOJppRXiZx";
    Solution sol(data);
    // Size buffer tightly to `n` to verify staging avoids out-of-bounds writes.
    std::vector<char> buf(42uz, '\0');
    const int cnt = sol.read(buf, 42);
    ctx.expect_eq(cnt, 42);
    ctx.expect_eq(std::string(buf.begin(), buf.begin() + cnt),
                  data.substr(0uz, 42uz));
  });

  ctx.test("exact multiples of 4 and exact file size", [&] {
    Solution sol("abcdefgh");
    std::vector<char> buf(8uz, '\0');
    const int cnt = sol.read(buf, 8);
    ctx.expect_eq(cnt, 8);
    ctx.expect_eq(std::string(buf.begin(), buf.begin() + cnt), "abcdefgh");
  });

  ctx.test("empty file and zero request", [&] {
    Solution empty_sol("");
    std::vector<char> buf(10uz, '\0');
    ctx.expect_eq(empty_sol.read(buf, 5), 0);

    Solution non_empty_sol("abc");
    ctx.expect_eq(non_empty_sol.read(buf, 0), 0);
  });
}

}  // namespace
