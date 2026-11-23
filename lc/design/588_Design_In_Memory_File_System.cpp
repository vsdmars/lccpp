// LeetCode 588: Design In Memory File System
// https://leetcode.com/problems/design-in-memory-file-system/
//
// Problem:
//   Design an in-memory hierarchical file system supporting four operations on
//   absolute Unix-style paths (starting with '/' and without trailing '/'
//   except for root "/"):
//     - ls(path): If path refers to a file, return a single-element list
//       containing that file's name. If path refers to a directory, return all
//       direct child file and subdirectory names in lexicographical order.
//     - mkdir(path): Create the directory at path along with any missing
//       intermediate parent directories.
//     - addContentToFile(filePath, content): Create the file (and any missing
//       parent directories) if absent, or append `content` to its existing
//       contents.
//     - readContentFromFile(filePath): Return the current contents of the file
//       at filePath.
//
// Examples:
//   Example 1:
//     Input:  FileSystem(); ls("/"); mkdir("/a/b/c"); addContentToFile("/a/b/c/d",
//             "hello"); ls("/"); readContentFromFile("/a/b/c/d"); ls("/a/b/c/d")
//     Output: [[], null, null, ["a"], "hello", ["d"]]
//     Explanation:
//       - Initial ls("/") inspects root_.children, which is empty, and returns [].
//       - mkdir("/a/b/c") splits the path into ["a", "b", "c"] and allocates
//         missing Trie nodes along root_ -> a -> b -> c.
//       - addContentToFile("/a/b/c/d", "hello") creates child "d" under "c", sets
//         is_file = true, and appends "hello" to its content.
//       - ls("/") returns ["a"] from root_.children,
//         readContentFromFile("/a/b/c/d") returns "hello", and ls("/a/b/c/d") sees
//         is_file == true and returns ["d"].
//
//   Example 2:
//     Input:  FileSystem(); mkdir("/dir/zeta"); addContentToFile("/dir/beta", "1");
//             mkdir("/dir/alpha"); addContentToFile("/dir/gamma", "2"); ls("/dir");
//             ls("/dir/beta")
//     Output: [null, null, null, null, ["alpha", "beta", "gamma", "zeta"],
//             ["beta"]]
//     Explanation:
//       - Each mkdir and addContentToFile inserts child keys under node "dir" into
//         its std::map<std::string, std::unique_ptr<Node>, std::less<>>.
//       - Because std::map stores keys in lexicographical order, /dir/children
//         holds {"alpha", "beta", "gamma", "zeta"} regardless of insertion order.
//       - ls("/dir") iterates /dir/children directly in O(K) to return ["alpha",
//         "beta", "gamma", "zeta"], while ls("/dir/beta") detects is_file == true
//         and returns ["beta"].
//
//   Example 3:
//     Input:  FileSystem(); ls("/does/not/exist"); readContentFromFile("/no/file")
//     Output: [[], ""]
//     Explanation:
//       - ls("/does/not/exist") calls find_node, which fails to find "does" in
//         root_.children and returns nullptr without mutating the Trie, yielding
//         [].
//       - readContentFromFile("/no/file") similarly gets nullptr from find_node on
//         missing component "no" and returns "".
//
// Algorithm:
//   Represent the directory hierarchy as a path-component Trie rooted at `/`.
//   Each node stores a `is_file` flag, a `content` string (populated when the
//   node is a file), and a `std::map` of child names to owned child nodes.
//   Storing children in a balanced BST (`std::map` with transparent
//   `std::less<>`) keeps directory listings continuously sorted in
//   lexicographical order and allows zero-allocation `std::string_view`
//   lookups along path components.
//   Time Complexity:
//     - mkdir / addContentToFile / readContentFromFile: O(P * log C), where P
//       is the number of path components and C is the max directory fanout
//       (plus O(|content|) for appending/reading content).
//     - ls: O(P * log C + K), where K is the number of entries in the target
//       directory (no extra sorting needed).
//   Space Complexity: O(total stored path names + total file content size).
//
// Data Structures:
//   - Path-component Trie (`Node`): Tree representing nested directories and
//     files, with RAII ownership via `std::unique_ptr<Node>`.
//   - std::map<std::string, std::unique_ptr<Node>, std::less<>>: Sorted child
//     directory table supporting heterogeneous `std::string_view` lookup.
//
// Real-World Applications:
//   - Virtual File Systems (VFS) and tmpfs/ramfs implementations in operating
//     system kernels (Linux dentry/inode tree).
//   - Sandboxed in-memory filesystems for unit testing, build tools, WebAssembly
//     runtimes, and browser-based IDEs (e.g., memfs, VS Code virtual FS).
//   - Hierarchical configuration stores and distributed coordination trees
//     (Apache ZooKeeper, etcd hierarchical namespaces).
//
// Related LeetCode Problems:
//   - #208 Implement Trie [done] (Prefix Tree) (core trie node structure)
//   - #1166 Design File System [new] (path-value mapping with parent checks)
//   - #609 Find Duplicate File in System [done] (file path & content grouping)
//   - #71 Simplify Path [done] (Unix path component parsing)
//   - #642 Design Search Autocomplete System [done] (trie with ordered results)

#include "framework/lc_framework.h"

#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace {

// Hierarchical in-memory file system backed by a lexicographically ordered
// path-component Trie.
class FileSystem {
 public:
  FileSystem() = default;

  // Lists direct children of a directory in lexicographical order, or returns
  // {filename} if `path` points to a file.
  // Time: O(P * log C + K). Space: O(K).
  [[nodiscard]] std::vector<std::string> ls(std::string_view path) const {
    const Node* node = find_node(path);
    if (node == nullptr) {
      return {};
    }

    if (node->is_file) {
      const std::vector<std::string_view> parts = split_path(path);
      if (parts.empty()) {
        return {};
      }
      return {std::string(parts.back())};
    }

    // Children are stored in std::map, so iteration yields lexicographical
    // order directly without sorting.
    std::vector<std::string> entries;
    entries.reserve(node->children.size());
    for (const auto& entry : node->children) {
      entries.push_back(entry.first);
    }
    return entries;
  }

  // Creates the directory at `path` and any missing parent directories.
  // Time: O(P * log C). Space: O(P) for newly created nodes.
  void mkdir(std::string_view path) { traverse_or_create(path); }

  // Creates the file at `file_path` (including missing parent directories) if
  // needed and appends `content` to it.
  // Time: O(P * log C + |content|). Space: O(P + |content|).
  void addContentToFile(std::string_view file_path, std::string_view content) {
    Node* node = traverse_or_create(file_path);
    node->is_file = true;
    node->content.append(content);
  }

  // Returns the stored content of `file_path`, or "" if absent or a directory.
  // Time: O(P * log C + |content|). Space: O(|content|).
  [[nodiscard]] std::string readContentFromFile(
      std::string_view file_path) const {
    const Node* node = find_node(file_path);
    if (node == nullptr || !node->is_file) {
      return {};
    }
    return node->content;
  }

 private:
  struct Node {
    bool is_file = false;
    std::string content;
    // Ordered map with transparent comparator enables string_view lookup and
    // keeps entries sorted for ls().
    std::map<std::string, std::unique_ptr<Node>, std::less<>> children;
  };

  // Splits an absolute Unix path into non-empty component views (returns empty
  // vector for "/").
  static std::vector<std::string_view> split_path(std::string_view path) {
    std::vector<std::string_view> parts;
    std::size_t pos = 0uz;
    while (pos < path.size()) {
      while (pos < path.size() && path[pos] == '/') {
        ++pos;
      }
      if (pos >= path.size()) {
        break;
      }
      const std::size_t slash = path.find('/', pos);
      if (slash == std::string_view::npos) {
        parts.push_back(path.substr(pos));
        break;
      }
      parts.push_back(path.substr(pos, slash - pos));
      pos = slash + 1uz;
    }
    return parts;
  }

  // Walks from root along `path`, creating intermediate nodes as needed.
  Node* traverse_or_create(std::string_view path) {
    Node* curr = &root_;
    for (const std::string_view part : split_path(path)) {
      auto it = curr->children.find(part);
      if (it == curr->children.end()) {
        it = curr->children.emplace(std::string(part), std::make_unique<Node>())
                 .first;
      }
      curr = it->second.get();
    }
    return curr;
  }

  // Walks from root along `path` without mutating; returns nullptr if missing.
  [[nodiscard]] const Node* find_node(std::string_view path) const {
    const Node* curr = &root_;
    for (const std::string_view part : split_path(path)) {
      const auto it = curr->children.find(part);
      if (it == curr->children.end()) {
        return nullptr;
      }
      curr = it->second.get();
    }
    return curr;
  }

  Node root_;
};

LC_PROBLEM(588, "Design In Memory File System", "design", ctx) {
  ctx.test("leetcode_example_1", [&] {
    FileSystem fs;
    ctx.expect_eq(fs.ls("/"), std::vector<std::string>{});
    fs.mkdir("/a/b/c");
    fs.addContentToFile("/a/b/c/d", "hello");
    ctx.expect_eq(fs.ls("/"), std::vector<std::string>{"a"});
    ctx.expect_eq(fs.readContentFromFile("/a/b/c/d"), std::string("hello"));
    ctx.expect_eq(fs.ls("/a/b/c/d"), std::vector<std::string>{"d"});
  });

  ctx.test("python_main_trace_append_and_sort", [&] {
    FileSystem fs;
    fs.addContentToFile("/c/d/e", "damn me");
    fs.addContentToFile("/c/d/e", "damn twice");
    ctx.expect_eq(fs.ls("/c/d"), std::vector<std::string>{"e"});
    ctx.expect_eq(fs.readContentFromFile("/c/d/e"),
                  std::string("damn medamn twice"));
    fs.mkdir("/a/b/c");
    ctx.expect_eq(fs.ls("/"), std::vector<std::string>{"a", "c"});
    ctx.expect_eq(fs.ls("/a"), std::vector<std::string>{"b"});
  });

  ctx.test("lexicographical_mix_of_files_and_dirs", [&] {
    FileSystem fs;
    fs.mkdir("/dir/zeta");
    fs.addContentToFile("/dir/beta", "1");
    fs.mkdir("/dir/alpha");
    fs.addContentToFile("/dir/gamma", "2");
    ctx.expect_eq(fs.ls("/dir"),
                  std::vector<std::string>{"alpha", "beta", "gamma", "zeta"});
    ctx.expect_eq(fs.ls("/dir/beta"), std::vector<std::string>{"beta"});
  });

  ctx.test("missing_paths_safe", [&] {
    FileSystem fs;
    ctx.expect_eq(fs.ls("/does/not/exist"), std::vector<std::string>{});
    ctx.expect_eq(fs.readContentFromFile("/no/file"), std::string());
  });
}

}  // namespace
