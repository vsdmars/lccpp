// LeetCode 271: Encode And Decode Strings
// https://leetcode.com/problems/encode-and-decode-strings/
//
// Problem:
//   Design a stateless algorithm to encode a list of strings into a single
//   string so that it can be transmitted over a network and decoded back into
//   the exact original list of strings. Input strings may contain any of the
//   256 possible ASCII characters (including digits, delimiters, and NUL
//   bytes), and strings may be empty.
//
// Examples:
//   Example 1:
//     Input:  strs = ["Hello", "World"]
//     Output: encoded = "5#Hello5#World", decoded = ["Hello", "World"]
//     Explanation:
//       - encode appends each string's byte length, '#', and raw bytes, producing
//         "5#Hello5#World".
//       - decode starts at pos = 0, finds '#' at index 1, parses len = 5, extracts
//         s.substr(2, 5) ("Hello"), and sets pos = 7.
//       - At pos = 7, decode finds '#' at index 8, parses len = 5, extracts
//         s.substr(9, 5) ("World"), and reaches pos = 14 == s.size(), returning
//         ["Hello", "World"].
//
//   Example 2:
//     Input:  strs = [""]
//     Output: encoded = "0#", decoded = [""]
//     Explanation:
//       - encode reads str.size() == 0 and emits the length header "0#" followed by
//         zero payload bytes.
//       - decode starts at pos = 0, finds '#' at index 1, parses len = 0, and
//         appends s.substr(2, 0) ("").
//       - pos advances to 2 + 0 = 2 == s.size(), terminating the loop and returning
//         [""].
//
//   Example 3:
//     Input:  strs = ["~!~##!!", "5#Hello", "0#", "12#34#56", "#"]
//     Output: encoded = "7#~!~##!!7#5#Hello2#0#8#12#34#561##", decoded =
//             ["~!~##!!", "5#Hello", "0#", "12#34#56", "#"]
//     Explanation:
//       - encode prefixes each entry with its exact byte count and '#', yielding
//         "7#~!~##!!7#5#Hello2#0#8#12#34#561##".
//       - At pos = 0, decode reads len = 7 before index 1 and slices s.substr(2, 7)
//         ("~!~##!!"), jumping straight over embedded '#' characters to pos = 9.
//       - At pos = 9, decode reads len = 7 and slices s.substr(11, 7) ("5#Hello"),
//         then repeats at pos = 18 (len = 2 for "0#"), pos = 22 (len = 8 for
//         "12#34#56"), and pos = 32 (len = 1 for "#").
//       - After the final slice ends at pos = 35 == s.size(), decode returns all
//         five original strings unmodified.
//
// Algorithm:
//   Naive delimiter-based splitting (such as joining with a sentinel like
//   "~!~##!!") breaks whenever an input string itself contains the delimiter.
//   Instead, we use length-prefixed framing (chunked transfer encoding):
//     - Encode: for each string `s`, write `<s.size()>#<s_bytes>` where the
//       decimal length is terminated by a single `'#'` separator followed by
//       exactly `s.size()` raw payload bytes.
//     - Decode: starting at cursor `pos = 0`, locate the next `'#'` to parse
//       the decimal byte count `len`, extract the exact slice of `len` bytes
//       immediately following `'#'`, and advance `pos` past the payload.
//   Because the decoder jumps directly over `len` bytes without scanning the
//   payload, arbitrary characters inside strings (including `'#'`, digits,
//   empty strings, and embedded `'\0'`) are preserved losslessly.
//   Time Complexity:  O(N) for both encode and decode, where N is total bytes.
//   Space Complexity: O(N) for the serialized / deserialized output.
//
// Data Structures:
//   - `std::string`: contiguous byte buffer for the length-prefixed wire format.
//   - `std::string_view`: zero-copy cursor slicing during decoding.
//   - `std::vector<std::string>`: input and reconstructed string sequence.
//
// Real-World Applications:
//   - Network framing protocols: HTTP/1.1 chunked transfer encoding, Redis RESP
//     bulk strings (`$<len>\r\n<bytes>\r\n`), and Netstrings (djb).
//   - Binary serialization formats (Protocol Buffers length-delimited fields,
//     TLV / Type-Length-Value packet framing, gRPC message framing).
//   - IPC / RPC message batching where payloads contain arbitrary binary data.
//
// Related LeetCode Problems:
//   - #297 Serialize and Deserialize Binary Tree [done] (structured serialization/deserialization codec)
//   - #443 String Compression [done] (run-length encoding of character streams)
//   - #38 Count and Say [done] (count-prefixed string encoding transformation)
//   - #535 Encode and Decode TinyURL [done] (codec API design for string mapping)
//   - #604 Design Compressed String Iterator [new] (decoding length-annotated string streams)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Codec {
 public:
  // Encodes a list of strings into a single length-prefixed string using the
  // framing format `<len>#<payload>` for each entry.
  // Time: O(N), Space: O(N), where N is the total number of characters.
  std::string encode(const std::vector<std::string>& strs) {
    // Pre-reserve enough space for all payloads plus length headers to minimize
    // buffer reallocations.
    std::size_t estimated_size = 0uz;
    for (const std::string& str : strs) {
      estimated_size += str.size() + 4uz;
    }

    std::string encoded;
    encoded.reserve(estimated_size);

    for (const std::string& str : strs) {
      encoded.append(std::to_string(str.size()));
      encoded.push_back('#');
      encoded.append(str);
    }
    return encoded;
  }

  // Decodes a length-prefixed string produced by `encode` back into the exact
  // original vector of strings.
  // Invariant: at the start of each loop iteration, `pos` points to the first
  // digit of the next frame's decimal length prefix.
  // Time: O(N), Space: O(N).
  std::vector<std::string> decode(std::string_view s) {
    std::vector<std::string> decoded;
    std::size_t pos = 0uz;

    while (pos < s.size()) {
      const std::size_t delim = s.find('#', pos);
      if (delim == std::string_view::npos) {
        break;
      }

      // Parse the decimal byte count between `pos` and `delim`.
      std::size_t len = 0uz;
      for (std::size_t i = pos; i < delim; ++i) {
        len = (len * 10uz) + static_cast<std::size_t>(s[i] - '0');
      }

      const std::size_t payload_start = delim + 1uz;
      // Guard against truncated/malformed frames before slicing.
      if (len > s.size() - payload_start) {
        break;
      }

      // Copy the exact `len` payload bytes without inspecting their contents.
      decoded.emplace_back(s.substr(payload_start, len));
      pos = payload_start + len;
    }

    return decoded;
  }
};

LC_PROBLEM(271, "Encode And Decode Strings", "string", ctx) {
  Codec codec;

  auto expect_roundtrip = [&](const std::vector<std::string>& input) {
    const std::string wire = codec.encode(input);
    const std::vector<std::string> restored = codec.decode(wire);
    ctx.expect_eq(restored, input);
  };

  ctx.test("example_1_hello_world",
           [&] { expect_roundtrip({"Hello", "World"}); });

  ctx.test("example_2_single_empty_string", [&] { expect_roundtrip({""}); });

  ctx.test("empty_list_and_multiple_empty_strings", [&] {
    expect_roundtrip({});
    expect_roundtrip({"", "", ""});
  });

  ctx.test("adversarial_delimiters_and_digits", [&] {
    // Includes the framing delimiter '#', digits, and the naive Python
    // sentinel "~!~##!!" to verify complete delimiter safety.
    expect_roundtrip({"~!~##!!", "5#Hello", "0#", "12#34#56", "#"});
  });

  ctx.test("all_ascii_and_embedded_nul", [&] {
    std::string all_bytes;
    all_bytes.reserve(256uz);
    for (int code = 0; code < 256; ++code) {
      all_bytes.push_back(static_cast<char>(code));
    }
    const std::string with_nul("abc\0def", 7uz);
    expect_roundtrip({all_bytes, with_nul, ""});
  });
}

}  // namespace
