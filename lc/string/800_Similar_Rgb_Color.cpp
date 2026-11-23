// LeetCode 800: Similar Rgb Color
// https://leetcode.com/problems/similar-rgb-color/
//
// Problem:
//   RGB colors in 6-digit hexadecimal form "#AABBCC" have a 3-digit shorthand
//   "#ABC" precisely when each color channel consists of two identical hex
//   digits (e.g., "#1155cc" == "#15c"). The similarity between two colors
//   "#ABCDEF" and "#UVWXYZ" is defined as:
//     -(AB - UV)^2 - (CD - WX)^2 - (EF - YZ)^2
//   where each pair is interpreted as a base-16 byte in [0, 255].
//   Given a 7-character color string `color`, return the 7-character shorthand-
//   representable color "#XXYYZZ" that maximizes similarity to `color`.
//
// Algorithm:
//   Because the similarity objective is a sum of independent negative squared
//   errors across the R, G, and B channels, maximizing total similarity reduces
//   to independently minimizing |v - 0xdd| for each channel value v in [0, 255]
//   over all 16 shorthand bytes 0x00, 0x11, ..., 0xff.
//   Notice that 0xdd = 16 * d + d = 17 * d for d in [0, 15]. Since adjacent
//   multiples of 17 are separated by the odd distance 17, every integer v in
//   [0, 255] has a unique nearest multiple of 17 given in closed form by:
//     d = (v + 8) / 17
//   (remainder r in [0, 8] rounds down to d; r in [9, 16] rounds up to d + 1).
//   Parse each 2-hex-digit channel, compute its nearest digit d, and emit the
//   repeated hex character twice.
//   Time Complexity:  O(1) — fixed 3 color channels.
//   Space Complexity: O(1) — fixed 7-character output string.
//
// Data Structures:
//   - Constant hex lookup string ("0123456789abcdef") and a 7-byte std::string
//     result buffer.
//
// Real-World Applications:
//   - CSS minifiers and design-token compilers snapping 24-bit hex colors to
//     compact 12-bit shorthand "#rgb" palettes with minimal perceptual loss.
//   - Color quantization and dithering for low-bit-depth displays (RGB444 /
//     web-safe palettes) and embedded OLED/LCD controllers.
//   - Nearest-lattice-point rounding in uniform scalar quantizers used by audio
//     and image codecs.
//
// Related LeetCode Problems:
//   - #405 Convert a Number to Hexadecimal [done] (hex digit encoding)
//   - #468 Validate IP Address [done] (structured hexadecimal token parsing)
//   - #1309 Decrypt String from Alphabet to Integer Mapping [new] (fixed-width token decoding)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace {

class Solution {
 public:
  // Returns the 7-character shorthand-representable color "#XXYYZZ" with the
  // highest similarity to `color`.
  // Time: O(1), Space: O(1).
  std::string similarRGB(std::string_view color) {
    std::string result;
    result.reserve(kColorLength);
    result.push_back('#');

    // Independently round each 2-hex-digit channel (R, G, B) to its nearest
    // multiple of 0x11 (decimal 17).
    for (std::size_t pos = 1uz; pos < kColorLength; pos += 2uz) {
      const int channel =
          (hex_to_int(color[pos]) * kHexBase) + hex_to_int(color[pos + 1uz]);
      // Half of 17 is 8.5, so integer division (channel + 8) / 17 yields the
      // unique closest shorthand digit d in [0, 15].
      const int nearest_digit = (channel + kHalfStep) / kShorthandStep;
      const char hex_char = kHexDigits[static_cast<std::size_t>(nearest_digit)];
      result.push_back(hex_char);
      result.push_back(hex_char);
    }

    return result;
  }

 private:
  static constexpr std::size_t kColorLength = 7;
  static constexpr int kHexBase = 16;
  static constexpr int kShorthandStep = 17;  // 0x11 == 17.
  static constexpr int kHalfStep = 8;        // floor(17 / 2).
  static constexpr std::string_view kHexDigits = "0123456789abcdef";

  // Converts a single hexadecimal character ('0'-'9', 'a'-'f', 'A'-'F') to its
  // integer value in [0, 15].
  static int hex_to_int(char ch) {
    if (ch >= '0' && ch <= '9') {
      return ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
      return (ch - 'a') + 10;
    }
    return (ch - 'A') + 10;
  }
};

LC_PROBLEM(800, "Similar Rgb Color", "string", ctx) {
  Solution sol;

  ctx.test("example_1_09f166", [&] {
    ctx.expect_eq(sol.similarRGB("#09f166"), "#11ee66");
  });

  ctx.test("example_2_4e3fe1", [&] {
    ctx.expect_eq(sol.similarRGB("#4e3fe1"), "#5544dd");
  });

  ctx.test("already_shorthand_extremes", [&] {
    ctx.expect_eq(sol.similarRGB("#000000"), "#000000");
    ctx.expect_eq(sol.similarRGB("#ffffff"), "#ffffff");
  });

  ctx.test("rounding_threshold_boundary_8_vs_9", [&] {
    // 0x08 (8) is closer to 0x00 (dist 8) than to 0x11 (dist 9).
    ctx.expect_eq(sol.similarRGB("#080808"), "#000000");
    // 0x09 (9) is closer to 0x11 (dist 8) than to 0x00 (dist 9).
    ctx.expect_eq(sol.similarRGB("#090909"), "#111111");
  });

  ctx.test("high_channel_near_ff", [&] {
    // 0xf7 (247) is distance 8 from 0xff (255) and distance 9 from 0xee (238).
    ctx.expect_eq(sol.similarRGB("#f7f6ee"), "#ffeeee");
  });
}

}  // namespace
