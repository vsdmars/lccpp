// LeetCode 1233: Remove Sub-Folders from the Filesystem
// https://leetcode.com/problems/remove-sub-folders-from-the-filesystem/
//
// Problem:
//   Given a list of unique Unix-style folder paths `folder`
//   (1 <= folder.length <= 4 * 10^4, 2 <= folder[i].length <= 100, each
//   starting with '/' and containing lowercase English letters and '/'),
//   remove every folder that is a sub-folder of another folder in the list and
//   return the remaining top-level folders in any order. A path `p` is a
//   sub-folder of `parent` iff `p` starts with `parent` followed immediately by
//   '/' (so "/a/b/ca" is not a sub-folder of "/a/b/c").
//
// Examples:
//   Example 1:
//     Input:  folder = ["/a", "/a/b", "/c/d", "/c/d/e", "/c/f"]
//     Output: ["/a", "/c/d", "/c/f"]
//     Explanation:
//       - Sorting folder lexicographically produces ["/a", "/a/b", "/c/d",
//         "/c/d/e", "/c/f"] so each parent precedes its sub-folders.
//       - "/a" is added to result, and "/a/b" is skipped because
//         is_subfolder("/a/b", "/a") verifies it starts with "/a" and has '/' at
//         index 2.
//       - "/c/d" does not start with "/a" so it becomes result.back(), after which
//         "/c/d/e" is skipped due to matching prefix "/c/d" and '/' at index 4.
//       - "/c/f" does not start with "/c/d" and is appended to result, yielding
//         ["/a", "/c/d", "/c/f"] (matching removeSubfoldersTrie, which stops DFS at
//         marked nodes "a", "d", and "f").
//
//   Example 2:
//     Input:  folder = ["/a/b/c", "/a/b/ca", "/a/b/d"]
//     Output: ["/a/b/c", "/a/b/ca", "/a/b/d"]
//     Explanation:
//       - Lexicographical sorting keeps ["/a/b/c", "/a/b/ca", "/a/b/d"] and pushes
//         "/a/b/c" into result.
//       - For "/a/b/ca", starts_with("/a/b/c") is true but path[6] is 'a' rather
//         than '/', so is_subfolder returns false and "/a/b/ca" is retained.
//       - "/a/b/d" does not start with "/a/b/ca" and is also retained, returning
//         ["/a/b/c", "/a/b/ca", "/a/b/d"].
//
//   Example 3:
//     Input:  folder = ["/a/b/c", "/a/b", "/a", "/x/y/z", "/x/y"]
//     Output: ["/a", "/x/y"]
//     Explanation:
//       - Sorting reorders the input into ["/a", "/a/b", "/a/b/c", "/x/y",
//         "/x/y/z"].
//       - "/a" is added to result, causing both "/a/b" and "/a/b/c" to be skipped
//         by is_subfolder because each has '/' at index 2.
//       - "/x/y" does not start with "/a" and is appended as the new result.back(),
//         which then filters out "/x/y/z" since "/x/y/z"[4] == '/'.
//       - The scan finishes with ["/a", "/x/y"].
//
// Algorithm:
//   Implements two approaches:
//     1. `removeSubfolders` (Sort + Linear Prefix Scan): Sorts `folder`
//        lexicographically so every sub-folder appears immediately after its
//        parent (or sibling sub-folders of that parent). Scans once, skipping
//        any path where `is_subfolder(path, result.back())` holds—checking
//        length, `starts_with`, and a '/' boundary with zero allocations.
//        - Time: O(N * L * log N), Space: O(1) auxiliary (beyond sort/output).
//     2. `removeSubfoldersTrie` (Path-Segment Trie + Pruned DFS): Splits each
//        path on '/' and inserts its directory segments into a flat-pool trie,
//        marking complete folder nodes with `folder_idx`. A DFS from root 0
//        records the folder and returns immediately upon hitting any marked
//        node, pruning all descendant sub-folders.
//        - Time: O(N * L) average, Space: O(N * L).
//
// Data Structures:
//   - `std::vector<std::string>` and `std::string_view`: Hold sorted paths and
//     perform zero-allocation prefix/boundary checks in Approach 1.
//   - `std::vector<TrieNode>` (arena-allocated path-segment Trie): Each
//     `TrieNode` maps segment names to child indices via
//     `std::unordered_map<std::string, std::size_t>` and marks terminal
//     folders with `std::optional<std::size_t> folder_idx`.
//
// Real-World Applications:
//   - File-system watchers (inotify/FSEvents), backup tools, and rsync engines
//     collapsing overlapping nested directories into minimal watch roots.
//   - Cloud IAM policy engines and HTTP reverse-proxy routers deduplicating
//     subsumed resource or URL path prefixes.
//   - Build systems and static analyzers normalizing overlapping include or
//     exclude directory trees.
//
// Related LeetCode Problems:
//   - #208 Implement Trie [done] (Prefix Tree) (foundational trie generalized here to
//     '/'-delimited path segments)
//   - #648 Replace Words [new] (finding shortest root prefixes using a trie)
//   - #1948 Delete Duplicate Folders in System [new] (path-segment trie construction
//     and duplicate subtree pruning)
//   - #588 Design In-Memory File System [done] (hierarchical Unix paths in a trie)
//   - #720 Longest Word in Dictionary [done] (prefix chains over sorted strings/trie)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

// Approach 1: Sort + Linear Prefix Scan.
// After lexicographical sorting, any sub-folder immediately follows its parent
// folder (or another sub-folder of the same parent), so we only need to check
// against the last retained root folder using a zero-allocation '/' boundary
// check.
// Time: O(N * L * log N), Space: O(1) extra (excluding sort and output).
//
// Approach 2: Path-Segment Trie (`removeSubfoldersTrie`).
// Insert each folder's '/'-delimited components into a trie and DFS from the
// root, stopping whenever a terminal folder node is reached so all sub-folders
// beneath it are pruned.
// Time: O(N * L), Space: O(N * L).
class Solution {
 public:
  std::vector<std::string> removeSubfolders(std::vector<std::string> folder) {
    std::ranges::sort(folder);
    std::vector<std::string> result;
    result.reserve(folder.size());

    for (std::string& path : folder) {
      if (!result.empty() && is_subfolder(path, result.back())) {
        continue;
      }
      result.push_back(std::move(path));
    }
    return result;
  }

  std::vector<std::string> removeSubfoldersTrie(
      const std::vector<std::string>& folder) {
    std::vector<TrieNode> nodes(1uz);
    for (std::size_t i = 0uz; i < folder.size(); ++i) {
      insert_path(nodes, folder[i], i);
    }

    std::vector<std::string> result;
    collect_roots(nodes, 0uz, folder, result);
    return result;
  }

 private:
  struct TrieNode {
    std::unordered_map<std::string, std::size_t> children;
    std::optional<std::size_t> folder_idx;
  };

  static bool is_subfolder(std::string_view path, std::string_view parent) {
    return path.size() > parent.size() && path.starts_with(parent) &&
           path[parent.size()] == '/';
  }

  static void insert_path(std::vector<TrieNode>& nodes, std::string_view path,
                          std::size_t folder_idx) {
    std::size_t cur = 0uz;
    std::size_t pos = 1uz;  // Skip leading '/'.
    while (pos < path.size()) {
      const std::size_t slash = path.find('/', pos);
      const std::size_t end =
          (slash == std::string_view::npos) ? path.size() : slash;
      std::string segment(path.substr(pos, end - pos));
      auto [it, inserted] =
          nodes[cur].children.try_emplace(std::move(segment), nodes.size());
      const std::size_t next = it->second;
      if (inserted) {
        nodes.emplace_back();
      }
      cur = next;
      pos = (slash == std::string_view::npos) ? path.size() : (slash + 1uz);
    }
    nodes[cur].folder_idx = folder_idx;
  }

  static void collect_roots(const std::vector<TrieNode>& nodes,
                            std::size_t node_idx,
                            const std::vector<std::string>& folder,
                            std::vector<std::string>& result) {
    if (nodes[node_idx].folder_idx.has_value()) {
      result.push_back(folder[*nodes[node_idx].folder_idx]);
      return;  // Prune all sub-folders beneath this node.
    }
    for (const auto& entry : nodes[node_idx].children) {
      collect_roots(nodes, entry.second, folder, result);
    }
  }
};

LC_PROBLEM(1233, "Remove Sub Folders From The Filesystem", "trie", ctx) {
  auto verify_both = [&](const std::vector<std::string>& input,
                         const std::vector<std::string>& expected) {
    auto sorted_res = Solution().removeSubfolders(input);
    auto trie_res = Solution().removeSubfoldersTrie(input);
    std::ranges::sort(sorted_res);
    std::ranges::sort(trie_res);
    ctx.expect_eq(sorted_res, expected);
    ctx.expect_eq(trie_res, expected);
  };

  ctx.test("example_1", [&] {
    verify_both({"/a", "/a/b", "/c/d", "/c/d/e", "/c/f"},
                {"/a", "/c/d", "/c/f"});
  });

  ctx.test("example_2", [&] {
    verify_both({"/a", "/a/b/c", "/a/b/d"}, {"/a"});
  });

  ctx.test("example_3_shared_name_prefix_not_subfolder", [&] {
    verify_both({"/a/b/c", "/a/b/ca", "/a/b/d"},
                {"/a/b/c", "/a/b/ca", "/a/b/d"});
  });

  ctx.test("single_and_reverse_ordered_input", [&] {
    verify_both({"/a"}, {"/a"});
    verify_both({"/a/b/c", "/a/b", "/a", "/x/y/z", "/x/y"}, {"/a", "/x/y"});
  });
}

}  // namespace
