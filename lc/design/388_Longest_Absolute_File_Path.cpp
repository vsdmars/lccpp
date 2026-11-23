// LeetCode 388: Longest Absolute File Path
// https://leetcode.com/problems/longest-absolute-file-path/
//
// Problem:
//   A file system is serialized as a newline-delimited (`'\n'`) string where
//   each entry's nesting depth equals its count of leading tab characters
//   (`'\t'`). A name containing a dot (`'.'`) represents a file; otherwise it
//   represents a directory. Note that spaces are valid characters inside file
//   or directory names and must not be stripped. Return the character length
//   of the longest absolute path (joined by `'/'`) to any file, or 0 if no
//   file exists. Required time complexity: O(n).
//
// Examples:
//   Example 1:
//     Input:  input = "dir\n\tsubdir1\n\tsubdir2\n\t\tfile.ext"
//     Output: 20
//     Explanation:
//       - Initialize depth_len = [0] and max_len = 0, then process "dir" (depth =
//         0, directory) to set depth_len[1] = 0 + 3 + 1 = 4.
//       - "\tsubdir1" (depth = 1, directory) sets depth_len[2] = 4 + 7 + 1 = 12,
//         and "\tsubdir2" (depth = 1, directory) overwrites depth_len[2] = 4 + 7 +
//         1 = 12.
//       - "\t\tfile.ext" (depth = 2, contains '.') is a file of length full_len =
//         depth_len[2] + 8 = 12 + 8 = 20 (for "dir/subdir2/file.ext"), updating
//         max_len = 20.
//
//   Example 2:
//     Input:  input =
//             "dir\n\tsubdir1\n\t\tfile1.ext\n\t\tsubsubdir1\n\tsubdir2\n\t\tsubsubdir2\n\t\t\tfile2.ext"
//     Output: 32
//     Explanation:
//       - "dir" (depth = 0) sets depth_len[1] = 4 and "\tsubdir1" (depth = 1) sets
//         depth_len[2] = 4 + 7 + 1 = 12.
//       - "\t\tfile1.ext" (depth = 2, file) computes full_len = 12 + 9 = 21
//         ("dir/subdir1/file1.ext") so max_len = 21; "\t\tsubsubdir1" (depth = 2,
//         directory) sets depth_len[3] = 12 + 10 + 1 = 23.
//       - "\tsubdir2" (depth = 1) overwrites depth_len[2] = 4 + 7 + 1 = 12, and
//         "\t\tsubsubdir2" (depth = 2) overwrites depth_len[3] = 12 + 10 + 1 = 23.
//       - "\t\t\tfile2.ext" (depth = 3, file) computes full_len = depth_len[3] + 9
//         = 23 + 9 = 32 ("dir/subdir2/subsubdir2/file2.ext"), updating max_len =
//         32.
//
//   Example 3:
//     Input:  input = "dir\n        file.txt"
//     Output: 16
//     Explanation:
//       - "dir" (depth = 0, directory) sets depth_len[1] = 0 + 3 + 1 = 4.
//       - "        file.txt" has 0 leading '\t' characters, so depth = 0 and all 8
//         leading spaces remain part of name (name.size() = 16).
//       - Because name contains '.', full_len = depth_len[0] + 16 = 0 + 16 = 16,
//         returning 16 as a top-level file rather than nesting under "dir".
//
// Algorithm:
//   Stream through the input line by line using `std::string_view` slices
//   (zero copies) while maintaining a depth-indexed prefix-length stack
//   `depth_len`:
//     - Invariant: `depth_len[d]` holds the character length of the active
//       parent directory path at depth `d` INCLUDING a trailing `'/'` (with
//       `depth_len[0] = 0` for the virtual root).
//     - For each line, count leading `'\t'` characters to obtain `depth`, and
//       take the remainder as `name`.
//     - The full path length of this entry is `depth_len[depth] + name.size()`.
//     - If `name` contains `'.'`, it is a file: update `max_len`.
//     - Otherwise it is a directory: store `full_len + 1` (accounting for the
//       trailing `'/'` separator) at `depth_len[depth + 1]`.
//   Because every character in `input` is inspected a constant number of times
//   and no path strings are materialized, this achieves strict O(n) time.
//   Time Complexity: O(n), where n = `input.size()`.
//   Space Complexity: O(d), where d is the maximum directory nesting depth.
//
// Data Structures:
//   - `std::vector<std::size_t>` (`depth_len`): compact depth-indexed stack of
//     cumulative directory path lengths.
//   - `std::string_view`: zero-allocation line and entry-name slicing.
//
// Real-World Applications:
//   - Static file-tree scanners and backup utilities checking OS `PATH_MAX`
//     limits (e.g., Windows 260-character MAX_PATH compliance checks).
//   - Indentation-sensitive parsers (YAML, Python ASTs, Makefile/outline
//     trees) tracking nested scope paths in a single linear pass.
//   - Build systems and archive unpackers resolving hierarchical manifest
//     entries without materializing intermediate strings.
//
// Related LeetCode Problems:
//   - #71 Simplify Path [done] (canonicalizing slash-delimited file paths)
//   - #588 Design In-Memory File System [done] (trie-based file system design)
//   - #609 Find Duplicate File in System [done] (parsing directory/file trees)
//   - #1166 Design File System [new] (hierarchical path-value store)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the character length of the longest absolute path to a file in the
  // tab-indented file-system serialization `input`, or 0 if no file exists.
  // Time: O(n), Space: O(max_depth).
  int lengthLongestPath(std::string_view input) {
    // depth_len[d] = length of the current ancestor prefix before a depth-d
    // entry (including trailing '/'), with depth_len[0] = 0 for top-level.
    std::vector<std::size_t> depth_len = {0};
    std::size_t max_len = 0;

    std::size_t pos = 0;
    while (pos < input.size()) {
      const std::size_t nl = input.find('\n', pos);
      const std::size_t line_end =
          (nl == std::string_view::npos) ? input.size() : nl;
      const std::string_view line = input.substr(pos, line_end - pos);

      // Count leading '\t' only; spaces belong to the entry name itself.
      std::size_t depth = 0;
      while (depth < line.size() && line[depth] == '\t') {
        ++depth;
      }
      const std::string_view name = line.substr(depth);

      // Guard against unexpected depth jumps in malformed inputs.
      if (depth >= depth_len.size()) {
        depth_len.resize(depth + 1, 0);
      }

      const std::size_t full_len = depth_len[depth] + name.size();
      if (name.contains('.')) {
        // File entry: record its complete path length (no trailing '/').
        max_len = std::max(max_len, full_len);
      } else {
        // Directory entry: store length + 1 for the '/' before any child.
        if (depth + 1 >= depth_len.size()) {
          depth_len.resize(depth + 2, 0);
        }
        depth_len[depth + 1] = full_len + 1;
      }

      if (nl == std::string_view::npos) {
        break;
      }
      pos = nl + 1;
    }

    return static_cast<int>(max_len);
  }
};

LC_PROBLEM(388, "Longest Absolute File Path", "design", ctx) {
  ctx.test("leetcode example 1", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.lengthLongestPath("dir\n\tsubdir1\n\tsubdir2\n\t\tfile.ext"), 20);
  });

  ctx.test("leetcode example 2", [&] {
    Solution sol;
    ctx.expect_eq(sol.lengthLongestPath(
                      "dir\n\tsubdir1\n\t\tfile1.ext\n\t\tsubsubdir1\n"
                      "\tsubdir2\n\t\tsubsubdir2\n\t\t\tfile2.ext"),
                  32);
  });

  ctx.test("python build example", [&] {
    Solution sol;
    ctx.expect_eq(sol.lengthLongestPath("dir\n\tfile.txt"), 12);
  });

  ctx.test("longer shallow path beats deeper shorter path", [&] {
    Solution sol;
    ctx.expect_eq(sol.lengthLongestPath(
                      "a\n\taa\n\t\taaa\n\t\t\tfile1.txt\n"
                      "aaaaaaaaaaaaaaaaaaaaa\n\tsth.png"),
                  29);
  });

  ctx.test("spaces in names are preserved and not treated as tabs", [&] {
    Solution sol;
    // "        file.txt" has 8 leading spaces (depth 0, not inside "dir").
    ctx.expect_eq(sol.lengthLongestPath("dir\n        file.txt"), 16);
    // Space inside nested file name is counted in length: "dir/ file.txt" = 13.
    ctx.expect_eq(sol.lengthLongestPath("dir\n\t file.txt"), 13);
  });

  ctx.test("no files, empty input, and top-level file", [&] {
    Solution sol;
    ctx.expect_eq(sol.lengthLongestPath(""), 0);
    ctx.expect_eq(sol.lengthLongestPath("a"), 0);
    ctx.expect_eq(sol.lengthLongestPath("dir\n\tsubdir1\n\tsubdir2"), 0);
    ctx.expect_eq(sol.lengthLongestPath("file1.txt\nfile2.txt\nlongfile.txt"),
                  12);
  });
}

}  // namespace
