// LeetCode 158: Read N Characters Given Read4 II Call Multiple Times
// https://leetcode.com/problems/read-n-characters-given-read4-ii-call-multiple-times/
//
// Problem:
//   Given a file and assuming you can only read it through a provided API
//   `int read4(char* buf4)` that reads up to 4 consecutive characters at a
//   time and returns the actual count read, implement `int read(char* buf,
//   int n)` to read `n` characters into `buf` and return the number of
//   characters actually read. Unlike LeetCode #157, `read` may be called
//   multiple times on the same file, so leftover bytes fetched by `read4`
//   must be preserved across calls.
//
// Algorithm:
//   Maintain a persistent 4-byte internal buffer `internal_buf_` along with
//   two cursors: `buf_head_` (index of the next unread cached byte) and
//   `buf_tail_` (one-past-the-last valid cached byte).
//   In `read(buf, n)`, loop until `written == n`:
//     1. If `buf_head_ == buf_tail_`, the internal cache is exhausted; call
//        `read4(internal_buf_.data())` to refill it. If `read4` returns 0, the
//        underlying stream hit EOF, so break immediately. Otherwise reset
//        `buf_head_ = 0` and `buf_tail_ = fetched`.
//     2. Copy `internal_buf_[buf_head_]` into `buf[written]` and advance both
//        `buf_head_` and `written`.
//   Because `internal_buf_`, `buf_head_`, and `buf_tail_` persist as member
//   state, any unconsumed bytes from a 4-byte block remain ready for the next
//   `read` call.
//   Time Complexity:  O(n) per `read(buf, n)` call (amortized O(1) per byte).
//   Space Complexity: O(1) — fixed 4-byte internal buffer and two indices.
//
// Data Structures:
//   Fixed-size `std::array<char, 4>` ring/slice buffer with two `std::size_t`
//   cursors (`buf_head_`, `buf_tail_`) sitting between the block-oriented
//   `Reader4` source and the caller's destination buffer.
//
// Real-World Applications:
//   - C stdio (`FILE*` / `fread`) and C++ `std::streambuf` buffered I/O layers
//     bridging fixed-size OS page/block reads (`read(2)`) to arbitrary
//     application read lengths.
//   - Network socket / TLS record framing where decryption produces fixed or
//     chunked plaintext blocks consumed incrementally by higher-level parsers.
//   - Block-device and flash-storage drivers caching sector reads for
//     unaligned sub-sector client requests.
//
// Related LeetCode Problems:
//   - #157 Read N Characters Given Read4 [done] (single-call version without cross-call cache)
//   - #284 Peeking Iterator [done] (one-element lookahead buffering over an iterator stream)
//   - #251 Flatten 2D Vector [done] (stateful cursor tracking across chunked containers)
//   - #341 Flatten Nested List Iterator [done] (stateful stream consumption over nested structures)

#include "framework/lc_framework.h"

#include <array>
#include <cstddef>
#include <string>
#include <utility>

namespace {

// Mock judge base class modeling LeetCode's hidden `Reader4` file-reading API.
class Reader4 {
 public:
  explicit Reader4(std::string file_content) : file_(std::move(file_content)) {}

  // Reads up to 4 consecutive characters from the backing file into `buf4` and
  // returns the number of characters actually read (0 once EOF is reached).
  int read4(char* buf4) {
    int count = 0;
    while (count < kChunkSize && file_pos_ < file_.size()) {
      buf4[count] = file_[file_pos_];
      ++count;
      ++file_pos_;
    }
    return count;
  }

 private:
  static constexpr int kChunkSize = 4;
  std::string file_;
  std::size_t file_pos_ = 0uz;
};

// Buffered stream reader built atop `Reader4` that preserves unread bytes
// across multiple `read` invocations.
class Solution : private Reader4 {
 public:
  explicit Solution(std::string file_content)
      : Reader4(std::move(file_content)) {}

  // Reads up to `n` characters into `buf`, reusing any leftover bytes cached
  // from prior `read4` calls before fetching fresh 4-byte chunks.
  //
  // Invariant: `internal_buf_[buf_head_ .. buf_tail_)` holds unread bytes
  // already pulled from the underlying file in exact stream order.
  //
  // Complexity: O(n) time per call, O(1) extra space.
  int read(char* buf, int n) {
    if (buf == nullptr || n <= 0) {
      return 0;
    }

    int written = 0;
    while (written < n) {
      // Refill the internal 4-byte cache whenever all cached bytes have been
      // drained.
      if (buf_head_ == buf_tail_) {
        const int fetched = read4(internal_buf_.data());
        if (fetched == 0) {
          // Underlying file reached EOF; return however many bytes we wrote.
          break;
        }
        buf_head_ = 0uz;
        buf_tail_ = static_cast<std::size_t>(fetched);
      }

      buf[written] = internal_buf_[buf_head_];
      ++buf_head_;
      ++written;
    }

    return written;
  }

 private:
  static constexpr std::size_t kInternalBufSize = 4uz;
  std::array<char, kInternalBufSize> internal_buf_{};
  std::size_t buf_head_ = 0uz;
  std::size_t buf_tail_ = 0uz;
};

LC_PROBLEM(158, "Read N Characters Given Read4 II Call Multiple Times",
           "two_pointers", ctx) {
  auto read_chunk = [](Solution& sol, int n) {
    std::string out(static_cast<std::size_t>(n > 0 ? n : 0), '\0');
    const int actual = sol.read(out.data(), n);
    out.resize(static_cast<std::size_t>(actual));
    return out;
  };

  ctx.test("leetcode_example_1_multiple_small_reads", [&] {
    Solution sol("abc");
    ctx.expect_eq(read_chunk(sol, 1), std::string("a"));
    ctx.expect_eq(read_chunk(sol, 2), std::string("bc"));
    ctx.expect_eq(read_chunk(sol, 1), std::string(""));
  });

  ctx.test("leetcode_example_2_overread_then_eof", [&] {
    Solution sol("abc");
    ctx.expect_eq(read_chunk(sol, 4), std::string("abc"));
    ctx.expect_eq(read_chunk(sol, 1), std::string(""));
  });

  ctx.test("python_source_long_string_split_reads", [&] {
    const std::string payload =
        "WRBqHdrOkyIDsdRMwRSYLBfaCYEdgxPlrlNppfkOKcqNnuwSmbUcJISmKtXxvRBJTSFzfM"
        "fdRsfbnvhFSqWQaeCZFKlOJppRXiZx";
    Solution sol(payload);
    // Read first 99 bytes (as in the Python harness), then verify the final
    // remaining byte and subsequent EOF.
    ctx.expect_eq(read_chunk(sol, 99), payload.substr(0, 99));
    ctx.expect_eq(read_chunk(sol, 5), payload.substr(99));
    ctx.expect_eq(read_chunk(sol, 4), std::string(""));
  });

  ctx.test("cross_chunk_boundaries_and_zero_reads", [&] {
    Solution sol("abcdefghij");
    ctx.expect_eq(read_chunk(sol, 0), std::string(""));
    ctx.expect_eq(read_chunk(sol, 1), std::string("a"));
    ctx.expect_eq(read_chunk(sol, 0), std::string(""));
    ctx.expect_eq(read_chunk(sol, 1), std::string("b"));
    ctx.expect_eq(read_chunk(sol, 3), std::string("cde"));
    ctx.expect_eq(read_chunk(sol, 2), std::string("fg"));
    ctx.expect_eq(read_chunk(sol, 10), std::string("hij"));
    ctx.expect_eq(read_chunk(sol, 1), std::string(""));
  });

  ctx.test("empty_file", [&] {
    Solution sol("");
    ctx.expect_eq(read_chunk(sol, 4), std::string(""));
    ctx.expect_eq(read_chunk(sol, 1), std::string(""));
  });
}

}  // namespace
