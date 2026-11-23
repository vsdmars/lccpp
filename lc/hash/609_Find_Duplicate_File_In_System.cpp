// LeetCode 609: Find Duplicate File In System
// https://leetcode.com/problems/find-duplicate-file-in-system/
//
// Problem:
//   Given a list of directory info strings, each formatted as:
//     "dir_path f1.txt(content1) f2.txt(content2) ... fn.txt(contentn)"
//   return all groups of duplicate file paths in the file system, where a
//   duplicate group consists of at least two files sharing identical content.
//   Each output path must have the format "dir_path/file_name". Groups and
//   paths within groups may be returned in any order.
//
// Examples:
//   Example 1:
//     Input:  paths = ["root/a 1.txt(abcd) 2.txt(efgh)", "root/c 3.txt(abcd)",
//             "root/c/d 4.txt(efgh)", "root 4.txt(efgh)"]
//     Output: [["root/a/1.txt", "root/c/3.txt"], ["root/4.txt", "root/a/2.txt",
//             "root/c/d/4.txt"]]
//     Explanation:
//       - parseDirectoryEntry splits the first record into directory "root/a" and
//         two tokens, appending "root/a/1.txt" under key "abcd" and "root/a/2.txt"
//         under key "efgh".
//       - Parsing the remaining three records appends "root/c/3.txt" to
//         content_to_paths["abcd"] and appends "root/c/d/4.txt" and "root/4.txt" to
//         content_to_paths["efgh"].
//       - Collecting every bucket in content_to_paths with size >= 2 returns the
//         two duplicate file groups (order-independent).
//
//   Example 2:
//     Input:  paths = ["root/a 1.txt(same) 2.txt(same) 3.txt(diff)"]
//     Output: [["root/a/1.txt", "root/a/2.txt"]]
//     Explanation:
//       - parseDirectoryEntry extracts directory "root/a" and parses all three
//         tokens in the single record.
//       - content_to_paths maps "same" -> ["root/a/1.txt", "root/a/2.txt"] and
//         "diff" -> ["root/a/3.txt"].
//       - Filtering for buckets of size >= 2 skips "diff" (size 1) and returns
//         [["root/a/1.txt", "root/a/2.txt"]].
//
//   Example 3:
//     Input:  paths = ["root/a 1.txt(abcd) 2.txt(efsfgh)", "root/c 3.txt(abdfcd)",
//             "root/c/d 4.txt(efggdfh)"]
//     Output: []
//     Explanation:
//       - parseDirectoryEntry inserts each of the 4 files into its own distinct
//         content key ("abcd", "efsfgh", "abdfcd", "efggdfh") in content_to_paths.
//       - Every bucket has size 1 (< 2), so no groups are emitted and findDuplicate
//         returns an empty vector.
//
// Algorithm:
//   1. Parse each directory record with zero-copy std::string_view slicing:
//      extract the leading directory path (up to the first space), then parse
//      each subsequent `filename(content)` token by locating '(' and ')'.
//   2. Insert `"dir_path/filename"` into an unordered hash map keyed by file
//      content (`content -> vector<full_path>`).
//   3. Collect and move every path list whose size is at least 2 into the
//      result vector.
//   Time Complexity:  O(T), where T is the total number of characters across
//                     all input strings.
//   Space Complexity: O(T) to store file contents and reconstructed paths in
//                     the grouping hash map.
//
// Data Structures:
//   - std::unordered_map<std::string, std::vector<std::string>>: groups full
//     file paths by their exact file content in O(1) expected amortized lookup
//     (scaled by content length).
//   - std::string_view: slices directory prefixes, filenames, and contents
//     without intermediate heap allocations during tokenization.
//
// Real-World Applications:
//   - Cloud storage and backup deduplication systems (e.g., Dropbox, ZFS,
//     Restic) identifying identical blobs — extended in production by bucketing
//     by file size first, then streaming chunked SHA-256 hashes, and finally
//     confirming byte-for-byte equality to eliminate hash-collision false
//     positives.
//   - Content-addressable storage (Git object store, Docker image layers)
//     keying immutable payloads by content digest.
//   - Build cache and asset pipeline deduplication across monorepos.
//
// Related LeetCode Problems:
//   - #49 Group Anagrams [done] (grouping strings into buckets by derived hash key)
//   - #652 Find Duplicate Subtrees [done] (detecting duplicate structures via serialization map)
//   - #71 Simplify Path [done] (parsing Unix-style filesystem path strings)
//   - #1948 Delete Duplicate Folders in System [new] (deduplicating entire directory subtrees)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // Groups all files that share identical content and returns every group
  // containing at least two file paths.
  // Time: O(total input characters), Space: O(total input characters).
  std::vector<std::vector<std::string>> findDuplicate(
      const std::vector<std::string>& paths) {
    std::unordered_map<std::string, std::vector<std::string>> content_to_paths;

    for (const std::string& entry : paths) {
      parseDirectoryEntry(entry, content_to_paths);
    }

    // Emit only content buckets that represent actual duplicates (>= 2 files).
    std::vector<std::vector<std::string>> duplicates;
    for (auto& bucket : content_to_paths) {
      if (bucket.second.size() >= 2uz) {
        duplicates.push_back(std::move(bucket.second));
      }
    }
    return duplicates;
  }

 private:
  // Parses a single `"dir f1(c1) f2(c2) ..."` record and appends each
  // `"dir/f_i"` path to `content_to_paths[c_i]`.
  static void parseDirectoryEntry(
      std::string_view entry,
      std::unordered_map<std::string, std::vector<std::string>>&
          content_to_paths) {
    const std::size_t first_space = entry.find(' ');
    if (first_space == std::string_view::npos) {
      return;
    }

    const std::string_view dir = entry.substr(0uz, first_space);
    std::size_t pos = first_space + 1uz;

    while (pos < entry.size()) {
      // Skip any consecutive separator spaces defensively.
      while (pos < entry.size() && entry[pos] == ' ') {
        ++pos;
      }
      if (pos >= entry.size()) {
        break;
      }

      const std::size_t next_space = entry.find(' ', pos);
      const std::string_view token =
          (next_space == std::string_view::npos)
              ? entry.substr(pos)
              : entry.substr(pos, next_space - pos);

      const std::size_t open_paren = token.find('(');
      const std::size_t close_paren =
          (open_paren == std::string_view::npos)
              ? std::string_view::npos
              : token.find(')', open_paren + 1uz);

      if (open_paren != std::string_view::npos &&
          close_paren != std::string_view::npos) {
        const std::string_view filename = token.substr(0uz, open_paren);
        const std::string_view content =
            token.substr(open_paren + 1uz, close_paren - (open_paren + 1uz));

        std::string full_path;
        full_path.reserve(dir.size() + 1uz + filename.size());
        full_path.append(dir);
        full_path.push_back('/');
        full_path.append(filename);

        content_to_paths[std::string(content)].push_back(std::move(full_path));
      }

      if (next_space == std::string_view::npos) {
        break;
      }
      pos = next_space + 1uz;
    }
  }
};

// Canonicalizes a list of duplicate groups (sorting paths within each group
// and sorting the groups lexicographically) for order-independent comparison.
std::vector<std::vector<std::string>> normalizeGroups(
    std::vector<std::vector<std::string>> groups) {
  for (auto& group : groups) {
    std::ranges::sort(group);
  }
  std::ranges::sort(groups);
  return groups;
}

LC_PROBLEM(609, "Find Duplicate File In System", "hash", ctx) {
  ctx.test("example_two_duplicate_groups", [&] {
    Solution sol;
    const std::vector<std::string> paths = {
        "root/a 1.txt(abcd) 2.txt(efgh)",
        "root/c 3.txt(abcd)",
        "root/c/d 4.txt(efgh)",
        "root 4.txt(efgh)",
    };
    const std::vector<std::vector<std::string>> expected = {
        {"root/a/1.txt", "root/c/3.txt"},
        {"root/4.txt", "root/a/2.txt", "root/c/d/4.txt"},
    };
    ctx.expect_eq(normalizeGroups(sol.findDuplicate(paths)),
                  normalizeGroups(expected));
  });

  ctx.test("all_unique_contents_returns_empty", [&] {
    Solution sol;
    // Matches the first build() case in the Python source.
    const std::vector<std::string> paths = {
        "root/a 1.txt(abcd) 2.txt(efsfgh)",
        "root/c 3.txt(abdfcd)",
        "root/c/d 4.txt(efggdfh)",
    };
    ctx.expect_true(sol.findDuplicate(paths).empty());
  });

  ctx.test("duplicates_within_same_directory", [&] {
    Solution sol;
    const std::vector<std::string> paths = {
        "root/a 1.txt(same) 2.txt(same) 3.txt(diff)",
    };
    const std::vector<std::vector<std::string>> expected = {
        {"root/a/1.txt", "root/a/2.txt"},
    };
    ctx.expect_eq(normalizeGroups(sol.findDuplicate(paths)), expected);
  });

  ctx.test("empty_input", [&] {
    Solution sol;
    ctx.expect_true(sol.findDuplicate({}).empty());
  });
}

}  // namespace
