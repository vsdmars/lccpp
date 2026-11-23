// LeetCode 393: UTF-8 Validation
// https://leetcode.com/problems/utf-8-validation/
//
// Problem:
//   Given an integer array `data` where only the least significant 8 bits of
//   each element represent one byte of input, determine whether `data` forms a
//   valid UTF-8 byte sequence under the following 1-to-4-byte prefix rules:
//     - 1-byte char: `0xxxxxxx`
//     - 2-byte char: `110xxxxx 10xxxxxx`
//     - 3-byte char: `1110xxxx 10xxxxxx 10xxxxxx`
//     - 4-byte char: `11110xxx 10xxxxxx 10xxxxxx 10xxxxxx`
//   Constraints: 1 <= data.length <= 2 * 10^4, 0 <= data[i] <= 255.
//
// Algorithm:
//   Run a single-pass deterministic finite-state machine over the byte stream,
//   maintaining `remaining` — the number of `10xxxxxx` continuation bytes still
//   required to finish the current multi-byte character:
//     - When `remaining == 0`, the current byte must be a valid leading byte
//       (`0xxxxxxx`, `110xxxxx`, `1110xxxx`, or `11110xxx`), which sets
//       `remaining` to 0, 1, 2, or 3 respectively. Any other pattern (such as
//       a stray continuation byte `10xxxxxx` or a 5+-bit prefix `11111xxx`) is
//       immediately rejected.
//     - When `remaining > 0`, the current byte must match `10xxxxxx`; otherwise
//       the sequence is invalid. Decrement `remaining`.
//   At the end of the array, the stream is valid if and only if `remaining == 0`
//   (no truncated multi-byte character).
//   Time Complexity:  O(n) — single linear scan with O(1) bitmask checks.
//   Space Complexity: O(1) — a single integer state counter.
//
// Data Structures:
//   Scalar integer state variable (`remaining`) encoding the 4-state UTF-8
//   decoder FSM (`0..3` expected continuation bytes).
//
// Real-World Applications:
//   Text decoders in web browsers, compilers, JSON/HTTP parsers, database
//   storage engines, and network protocol validators verifying well-formed
//   UTF-8 streams before processing.
//
// Related LeetCode Problems:
//   - #190 Reverse Bits [done] (bitmask inspection across fixed-width words)
//   - #191 Number of 1 Bits [done] (bit-level counting and prefix inspection)
//   - #405 Convert a Number to Hexadecimal [done] (byte/nibble bit-field parsing)
//   - #461 Hamming Distance [done] (bitwise comparison on integer operands)

#include "framework/lc_framework.h"

#include <cstdint>
#include <vector>

namespace {

class Solution {
 public:
  // Validates whether the least significant 8 bits of each element in `data`
  // form a sequence of valid 1-to-4-byte UTF-8 characters.
  // Invariant: before inspecting each byte, `remaining` equals the exact count
  // of `10xxxxxx` continuation bytes needed to complete the active character.
  // Time: O(n), Space: O(1).
  bool validUtf8(const std::vector<int>& data) {
    int remaining = 0;

    for (const int raw : data) {
      // Per specification, only the least significant 8 bits carry data.
      const std::uint32_t byte = static_cast<std::uint32_t>(raw) & 0xFFU;

      if (remaining == 0) {
        const int needed = leadingContinuationCount(byte);
        if (needed < 0) {
          return false;
        }
        remaining = needed;
      } else {
        // Every continuation byte must start with the two high bits `10`.
        if ((byte & 0xC0U) != 0x80U) {
          return false;
        }
        --remaining;
      }
    }

    // All started multi-byte sequences must have been completed.
    return remaining == 0;
  }

 private:
  // Decodes a UTF-8 leading byte into the number of continuation bytes that
  // must follow it (0..3), or returns -1 if the byte is not a valid lead byte.
  static int leadingContinuationCount(std::uint32_t byte) {
    if ((byte & 0x80U) == 0x00U) {
      return 0;  // 0xxxxxxx -> 1-byte ASCII character
    }
    if ((byte & 0xE0U) == 0xC0U) {
      return 1;  // 110xxxxx -> 2-byte character
    }
    if ((byte & 0xF0U) == 0xE0U) {
      return 2;  // 1110xxxx -> 3-byte character
    }
    if ((byte & 0xF8U) == 0xF0U) {
      return 3;  // 11110xxx -> 4-byte character
    }
    return -1;  // Invalid leading prefix (10xxxxxx or 11111xxx)
  }
};

LC_PROBLEM(393, "UTF 8 Validation", "bit_manipulation", ctx) {
  Solution sol;

  ctx.test("example_1_valid_2byte_then_1byte", [&] {
    // 11000101 10000010 00000001
    ctx.expect_true(sol.validUtf8({197, 130, 1}));
  });

  ctx.test("example_2_invalid_3byte_short_continuation", [&] {
    // 11101011 10001100 00000100
    ctx.expect_false(sol.validUtf8({235, 140, 4}));
  });

  ctx.test("mixed_valid_sequence",
           [&] { ctx.expect_true(sol.validUtf8({1, 197, 130, 3, 197, 130})); });

  ctx.test("valid_4byte_character", [&] {
    // 11110000 10100010 10001010 10010011
    ctx.expect_true(sol.validUtf8({240, 162, 138, 147}));
  });

  ctx.test("truncated_and_stray_continuation_and_5byte_prefix", [&] {
    ctx.expect_false(sol.validUtf8({197}));                   // truncated 2-byte
    ctx.expect_false(sol.validUtf8({145}));                   // stray 10xxxxxx
    ctx.expect_false(sol.validUtf8({248, 130, 130, 130, 130}));  // 11111000
    ctx.expect_false(sol.validUtf8({255}));                   // 11111111
  });

  ctx.test("high_bits_ignored_and_empty", [&] {
    ctx.expect_true(sol.validUtf8({256 + 197, 512 + 130, 256 + 1}));
    ctx.expect_true(sol.validUtf8({}));
  });
}

}  // namespace
