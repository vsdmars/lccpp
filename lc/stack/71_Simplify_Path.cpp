// LeetCode 71: Simplify Path
// https://leetcode.com/problems/simplify-path/
//
// Problem:
//   Given a Unix-style file path string `path`, convert it into its simplified
//   canonical path. Consecutive slashes ('/') collapse into a single slash,
//   '.' denotes the current directory and is ignored, '..' moves up one
//   directory level (clamped at root), and any other sequence of characters
//   (including '...' or '....') is treated as a valid directory/file name.
//   The simplified absolute path must start with a single '/' and have no
//   trailing '/' unless it is the root directory "/".
//
// Examples:
//   Example 1:
//     Input:  path = "/home/user/Documents/../Pictures"
//     Output: "/home/user/Pictures"
//     Explanation:
//       - Because path starts with '/', set is_absolute = true and initialize an
//         empty directory stack dirs.
//       - Scan '/'-delimited tokens and push "home", "user", and "Documents" onto
//         dirs.
//       - On token "..", pop "Documents" from dirs (leaving ["home", "user"]), then
//         push "Pictures".
//       - Reconstruct the canonical path with a leading '/' and single '/'
//         separators between dirs entries to return "/home/user/Pictures".
//
//   Example 2:
//     Input:  path = "/.../a/../b/c/../d/./"
//     Output: "/.../b/d"
//     Explanation:
//       - Push "..." as a normal directory name onto dirs since it is neither "."
//         nor "..", followed by "a".
//       - Pop "a" on the first "..", push "b" and "c" (dirs = ["...", "b", "c"]),
//         and pop "c" on the second "..".
//       - Push "d", ignore the single-dot token "." and trailing '/', and join
//         ["...", "b", "d"] with '/' to return "/.../b/d".
//
//   Example 3:
//     Input:  path = "/../"
//     Output: "/"
//     Explanation:
//       - Set is_absolute = true from the leading '/' and skip to the token "..".
//       - Because dirs is already empty, handling ".." is a no-op so dirs remains
//         empty at root.
//       - Skip the trailing '/' and reconstruct an absolute path from empty dirs,
//         returning "/".
//
// Algorithm:
//   Scan `path` once by splitting on '/' delimiters without allocating
//   intermediate component strings (using `std::string_view` slices). Maintain
//   a stack `dirs` of active path components:
//   - Ignore empty tokens (from leading, trailing, or consecutive slashes) and
//     single-dot tokens ".".
//   - On "..", pop the top component from `dirs` if non-empty (at root, ".."
//     is a no-op).
//   - Push all other tokens onto `dirs`.
//   Finally, join the surviving components in order with '/', prefixed by '/'
//   whenever the input path was absolute.
//   Time complexity: O(n), where n = path.size().
//   Space complexity: O(n) to store component views and the output string.
//
// Data Structures:
//   - std::vector<std::string_view> used as a LIFO directory stack so ".."
//     can unwind the most recent directory in O(1) without copying strings.
//
// Real-World Applications:
//   - OS kernels, VFS layers, and shells resolving canonical file paths
//     (POSIX `realpath` / `std::filesystem::weakly_canonical`).
//   - Web servers and reverse proxies normalizing request URLs to prevent
//     directory-traversal security vulnerabilities (`../../etc/passwd`).
//   - Build systems and static-site generators deduplicating asset paths.
//
// Related LeetCode Problems:
//   - #20 Valid Parentheses [done] (matching nested structure with a stack)
//   - #150 Evaluate Reverse Polish Notation [new] (token-stream stack eval)
//   - #388 Longest Absolute File Path [done] (stack-tracked directory depths)
//   - #1598 Crawler Log Folder [new] (directory-depth tracking with "../")

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace {

class Solution {
 public:
  // Simplifies a Unix-style file path into its canonical form.
  // Time: O(n), Space: O(n).
  std::string simplifyPath(std::string_view path) {
    if (path.empty()) {
      return "";
    }

    const bool is_absolute = path.front() == '/';
    // Stack of active directory/file name slices in order from root downward.
    std::vector<std::string_view> dirs;

    std::size_t pos = 0uz;
    while (pos < path.size()) {
      // Skip one or more consecutive '/' delimiters.
      while (pos < path.size() && path[pos] == '/') {
        ++pos;
      }
      if (pos == path.size()) {
        break;
      }

      // Extract the next path segment up to the next '/' or end of string.
      const std::size_t slash = path.find('/', pos);
      const std::size_t end =
          (slash == std::string_view::npos) ? path.size() : slash;
      const std::string_view token = path.substr(pos, end - pos);
      pos = end;

      if (token == ".") {
        continue;
      }
      if (token == "..") {
        // Move up one directory if possible; above root remains at root.
        if (!dirs.empty()) {
          dirs.pop_back();
        }
        continue;
      }
      dirs.push_back(token);
    }

    // Reconstruct the canonical path in a single pre-sized buffer.
    std::string result;
    result.reserve(path.size());
    if (is_absolute) {
      result.push_back('/');
    }
    for (std::size_t i = 0uz; i < dirs.size(); ++i) {
      if (i > 0uz) {
        result.push_back('/');
      }
      result.append(dirs[i]);
    }
    return result;
  }
};

LC_PROBLEM(71, "Simplify Path", "stack", ctx) {
  Solution sol;

  ctx.test("leetcode_examples", [&] {
    ctx.expect_eq(sol.simplifyPath("/home/"), "/home");
    ctx.expect_eq(sol.simplifyPath("/home//foo/"), "/home/foo");
    ctx.expect_eq(sol.simplifyPath("/home/user/Documents/../Pictures"),
                  "/home/user/Pictures");
    ctx.expect_eq(sol.simplifyPath("/../"), "/");
    ctx.expect_eq(sol.simplifyPath("/.../a/../b/c/../d/./"), "/.../b/d");
  });

  ctx.test("source_build_cases", [&] {
    ctx.expect_eq(sol.simplifyPath("/a/b/../c"), "/a/c");
    ctx.expect_eq(sol.simplifyPath("/a/./b/../../c/"), "/c");
    ctx.expect_eq(sol.simplifyPath("/abc/..."), "/abc/...");
    ctx.expect_eq(sol.simplifyPath("/.."), "/");
    ctx.expect_eq(sol.simplifyPath(".."), "");
    ctx.expect_eq(sol.simplifyPath("/"), "/");
  });

  ctx.test("edge_cases", [&] {
    ctx.expect_eq(sol.simplifyPath(""), "");
    ctx.expect_eq(sol.simplifyPath("////"), "/");
    ctx.expect_eq(sol.simplifyPath("/a//b////c/d//././/.."), "/a/b/c");
    ctx.expect_eq(sol.simplifyPath("/..hidden/.file"), "/..hidden/.file");
  });
}

}  // namespace
