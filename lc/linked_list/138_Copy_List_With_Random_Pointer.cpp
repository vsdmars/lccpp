// LeetCode 138: Copy List With Random Pointer
// https://leetcode.com/problems/copy-list-with-random-pointer/
//
// Problem:
//   Given a linked list of length `n` where each node contains an integer
//   value, a `next` pointer, and an additional `random` pointer that may point
//   to any node in the list or `nullptr`, construct and return a deep copy of
//   the list without mutating the original list's final structure.
//   Constraints: 0 <= n <= 1000, -10^4 <= Node.val <= 10^4.
//
// Examples:
//   Example 1:
//     Input:  head = [[7, null], [13, 0], [11, 4], [10, 2], [1, 0]]
//     Output: [[7, null], [13, 0], [11, 4], [10, 2], [1, 0]]
//     Explanation:
//       - Pass 1 allocates a clone after each original node, weaving `7 -> 7' -> 13
//         -> 13' -> 11 -> 11' -> 10 -> 10' -> 1 -> 1'`.
//       - Pass 2 wires `copy->random = cur->random->next` for non-null randoms:
//         `7'` stays `null`, `13'` points to `7'`, `11'` to `1'`, `10'` to `11'`,
//         and `1'` to `7'`.
//       - Pass 3 unweaves the chain into `copy_dummy.next` (`7' -> 13' -> 11' ->
//         10' -> 1'`) while restoring `7 -> 13 -> 11 -> 10 -> 1` in the original
//         list.
//       - Return `copy_dummy.next`, producing a pointer-disjoint deep copy matching
//         all values and random indices.
//
//   Example 2:
//     Input:  head = [[1, 1], [2, 1]]
//     Output: [[1, 1], [2, 1]]
//     Explanation:
//       - Pass 1 interleaves clones to form `1 -> 1' -> 2 -> 2'`.
//       - Pass 2 sets `1'->random = node(2)->next = 2'` and `2'->random =
//         node(2)->next = 2'` (a self-loop on index 1 of the copy).
//       - Pass 3 detaches `1' -> 2'` from `1 -> 2` and sets `copy_tail->next =
//         nullptr`, returning `[[1, 1], [2, 1]]`.
//
//   Example 3:
//     Input:  head = []
//     Output: []
//     Explanation:
//       - Check `head == nullptr` at the start of `copyRandomList`.
//       - Return `nullptr` (`[]`) immediately without allocating nodes or running
//         the three passes.
//
// Algorithm:
//   Three-pass in-place node interleaving (O(1) extra space, no hash map):
//   1. Interleave clones: for every original node `cur`, allocate its clone
//      `copy = new Node(cur->val)` and insert `copy` immediately after `cur`:
//        orig_0 -> copy_0 -> orig_1 -> copy_1 -> ...
//      This establishes an implicit O(1) mapping `clone(orig) == orig->next`.
//   2. Wire `random` pointers: walk each original node `cur`; if
//      `cur->random != nullptr`, set `cur->next->random = cur->random->next`
//      (the clone sitting directly after `cur->random`).
//   3. Unweave the two lists: separate the interleaved chain back into the
//      original list (`cur->next = copy->next`) and the standalone cloned list
//      anchored at a dummy head, restoring the original list completely.
//   Time Complexity: O(n) across three linear passes.
//   Space Complexity: O(1) auxiliary space beyond the n newly allocated nodes.
//
// Data Structures:
//   - Custom linked list (`Node`) with `next` and `random` pointers.
//   - Implicit pointer-interleaving adjacency replacing an explicit
//     `std::unordered_map<Node*, Node*>` lookup table.
//   - Stack-allocated dummy sentinel to cleanly extract the cloned list head.
//
// Real-World Applications:
//   - Deep-cloning object graphs with arbitrary cross-references in language
//     runtimes, garbage collectors (Cheney's forwarding-pointer copying GC),
//     and prototype/snapshot serializers.
//   - Duplicating control-flow graphs or ASTs with jump/symbol back-pointers
//     during compiler function inlining.
//   - Forking transaction/state-machine graphs in simulation and undo engines.
//
// Related LeetCode Problems:
//   - #133 Clone Graph [done] (deep-copying a general graph with a hash map)
//   - #1485 Clone Binary Tree With Random Pointer [done] (tree analogue with random links)
//   - #1490 Clone N-ary Tree [new] (deep-copying an N-ary hierarchy)
//   - #141 Linked List Cycle [done] (in-place pointer manipulation on lists)

#include "framework/lc_framework.h"

#include <cstddef>
#include <deque>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

// Standard LeetCode linked-list node augmented with an arbitrary random link.
struct Node {
  int val = 0;
  Node* next = nullptr;
  Node* random = nullptr;

  Node() = default;
  explicit Node(int x) : val(x) {}
  Node(int x, Node* nxt, Node* rnd) : val(x), next(nxt), random(rnd) {}
};

class Solution {
 public:
  // Deep-copies a linked list with arbitrary `random` pointers using O(1)
  // auxiliary space via temporary node interleaving.
  // Invariant: after Pass 1, every original node `u` satisfies
  // `u->next == clone(u)`, enabling O(1) random-pointer translation in Pass 2.
  // Time: O(n), Space: O(1) auxiliary.
  Node* copyRandomList(Node* head) {
    if (head == nullptr) {
      return nullptr;
    }

    // Pass 1: weave each cloned node immediately after its original node.
    for (Node* cur = head; cur != nullptr;) {
      Node* next_orig = cur->next;
      auto* copy = new Node(cur->val, next_orig, nullptr);
      cur->next = copy;
      cur = next_orig;
    }

    // Pass 2: translate every original `random` target `r` to `r->next`.
    for (Node* cur = head; cur != nullptr;) {
      Node* copy = cur->next;
      if (copy == nullptr) {
        break;
      }
      if (cur->random != nullptr) {
        copy->random = cur->random->next;
      }
      cur = copy->next;
    }

    // Pass 3: unweave the interleaved chain, restoring the original list and
    // extracting the newly constructed deep copy.
    Node copy_dummy(0);
    Node* copy_tail = &copy_dummy;
    for (Node* cur = head; cur != nullptr;) {
      Node* copy = cur->next;
      if (copy == nullptr) {
        break;
      }
      Node* next_orig = copy->next;

      copy_tail->next = copy;
      copy_tail = copy;

      cur->next = next_orig;
      cur = next_orig;
    }
    copy_tail->next = nullptr;

    return copy_dummy.next;
  }
};

// Descriptor for building test lists: {node value, optional random target idx}.
struct NodeSpec {
  int val = 0;
  std::optional<std::size_t> random_index;
};

// RAII arena managing the original test nodes.
struct NodeArena {
  std::deque<Node> nodes;

  Node* build(const std::vector<NodeSpec>& specs) {
    if (specs.empty()) {
      return nullptr;
    }
    std::vector<Node*> ptrs;
    ptrs.reserve(specs.size());
    for (const auto& spec : specs) {
      nodes.emplace_back(spec.val);
      ptrs.push_back(&nodes.back());
    }
    for (std::size_t i = 0uz; i < specs.size(); ++i) {
      if ((i + 1uz) < specs.size()) {
        ptrs[i]->next = ptrs[i + 1uz];
      }
      if (specs[i].random_index.has_value()) {
        ptrs[i]->random = ptrs[*specs[i].random_index];
      }
    }
    return ptrs.front();
  }
};

// RAII owner that frees heap-allocated nodes returned by copyRandomList.
struct NodeListGuard {
  Node* head = nullptr;

  explicit NodeListGuard(Node* h) : head(h) {}
  NodeListGuard(const NodeListGuard&) = delete;
  NodeListGuard& operator=(const NodeListGuard&) = delete;
  NodeListGuard(NodeListGuard&&) = delete;
  NodeListGuard& operator=(NodeListGuard&&) = delete;
  ~NodeListGuard() {
    Node* cur = head;
    while (cur != nullptr) {
      Node* nxt = cur->next;
      delete cur;
      cur = nxt;
    }
  }
};

// Checks that a single node's `random` link resolves to `expected_idx`.
bool random_matches(const Node* random_ptr,
                    const std::unordered_map<const Node*, std::size_t>& index,
                    std::optional<std::size_t> expected_idx) {
  if (!expected_idx.has_value()) {
    return random_ptr == nullptr;
  }
  const auto it = index.find(random_ptr);
  return it != index.end() && it->second == *expected_idx;
}

// Verifies that `copy` is a faithful, pointer-disjoint deep copy of `orig`
// and that `orig` itself still matches `expected`.
bool verify_deep_copy(const Node* orig, const Node* copy,
                      const std::vector<NodeSpec>& expected) {
  std::vector<const Node*> orig_nodes;
  std::vector<const Node*> copy_nodes;
  std::unordered_map<const Node*, std::size_t> orig_idx;
  std::unordered_map<const Node*, std::size_t> copy_idx;
  std::unordered_set<const Node*> orig_set;

  for (const Node* cur = orig; cur != nullptr; cur = cur->next) {
    orig_idx[cur] = orig_nodes.size();
    orig_set.insert(cur);
    orig_nodes.push_back(cur);
  }
  for (const Node* cur = copy; cur != nullptr; cur = cur->next) {
    if (orig_set.contains(cur)) {
      return false;  // Cloned list aliased an original node.
    }
    copy_idx[cur] = copy_nodes.size();
    copy_nodes.push_back(cur);
  }

  if (orig_nodes.size() != expected.size() ||
      copy_nodes.size() != expected.size()) {
    return false;
  }

  for (std::size_t i = 0uz; i < expected.size(); ++i) {
    if (orig_nodes[i]->val != expected[i].val ||
        copy_nodes[i]->val != expected[i].val) {
      return false;
    }
    if (!random_matches(orig_nodes[i]->random, orig_idx,
                        expected[i].random_index) ||
        !random_matches(copy_nodes[i]->random, copy_idx,
                        expected[i].random_index)) {
      return false;
    }
  }
  return true;
}

LC_PROBLEM(138, "Copy List With Random Pointer", "linked_list", ctx) {
  Solution sol;

  auto run_case = [&](const std::vector<NodeSpec>& specs) {
    NodeArena arena;
    Node* orig = arena.build(specs);
    NodeListGuard copied(sol.copyRandomList(orig));
    ctx.expect_true(verify_deep_copy(orig, copied.head, specs));
  };

  ctx.test("python_build_six_nodes", [&] {
    run_case({{.val = 3, .random_index = 5uz},
              {.val = 4, .random_index = 3uz},
              {.val = 4, .random_index = 5uz},
              {.val = 5, .random_index = 4uz},
              {.val = 6, .random_index = 1uz},
              {.val = 7, .random_index = 4uz}});
  });

  ctx.test("example_1_mixed_randoms", [&] {
    run_case({{.val = 7, .random_index = std::nullopt},
              {.val = 13, .random_index = 0uz},
              {.val = 11, .random_index = 4uz},
              {.val = 10, .random_index = 2uz},
              {.val = 1, .random_index = 0uz}});
  });

  ctx.test("example_2_self_pointing_random", [&] {
    run_case({{.val = 1, .random_index = 1uz},
              {.val = 2, .random_index = 1uz}});
  });

  ctx.test("example_3_duplicate_vals_and_nulls", [&] {
    run_case({{.val = 3, .random_index = std::nullopt},
              {.val = 3, .random_index = 0uz},
              {.val = 3, .random_index = std::nullopt}});
  });

  ctx.test("empty_and_single_nodes", [&] {
    run_case({});
    run_case({{.val = 9, .random_index = std::nullopt}});
    run_case({{.val = 9, .random_index = 0uz}});
  });
}

}  // namespace
