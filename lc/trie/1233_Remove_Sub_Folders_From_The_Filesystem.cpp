// https://leetcode.com/problems/remove-sub-folders-from-the-filesystem/
//
// Given a list of unique Unix-style folder paths (e.g. "/a/b"), remove all
// sub-folders of any other folder in the list and return the remaining
// top-level folders in any order.

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
