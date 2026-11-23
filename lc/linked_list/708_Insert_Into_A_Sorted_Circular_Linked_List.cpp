// LeetCode 708: Insert Into A Sorted Circular Linked List
// https://leetcode.com/problems/insert-into-a-sorted-circular-linked-list/
//
// Problem:
//   Given a pointer to an arbitrary node in a circular singly linked list
//   sorted in non-decreasing order and an integer `insertVal`, insert a new
//   node with value `insertVal` so that the list remains circular and sorted.
//   If the list is empty (`head == nullptr`), create a single-node circular
//   list pointing to itself and return it. Otherwise, return the original
//   `head` pointer. When multiple insertion positions are valid, any may be
//   chosen.
//
// Algorithm:
//   Single-pass circular scan handling four structural cases:
//   1. Empty list (`head == nullptr`): allocate `node` with `node->next = node`
//      and return it.
//   2. Interior sorted position: `curr->val <= insertVal <= curr->next->val` —
//      the new value fits directly between two adjacent nodes.
//   3. Wrap-around boundary (maximum -> minimum): `curr->val > curr->next->val`
//      and either `insertVal >= curr->val` (new maximum) or
//      `insertVal <= curr->next->val` (new minimum). Insert between the tail
//      maximum and head minimum.
//   4. Uniform values (all nodes share the same value, including 1-node lists):
//      after completing one full cycle back to `head`, none of the above
//      conditions fired, so every slot is equally valid; break and insert right
//      after `curr`.
//   Time Complexity: O(n), visiting each node in the cycle at most once.
//   Space Complexity: O(1) auxiliary space (only the single newly inserted
//   node is allocated).
//
// Data Structures:
//   - Circular singly linked list (`Node`): traversed with a single `curr`
//     pointer anchored against `head` to guarantee cycle termination.
//
// Real-World Applications:
//   - Consistent hashing rings (Dynamo, Cassandra, Memcached) where new
//     virtual server nodes are inserted into a sorted circular hash space.
//   - OS timer wheels and round-robin deadline schedulers maintaining sorted
//     cyclic event lists.
//   - Computational geometry cyclic polygon vertex insertion along sorted
//     angular order.
//
// Related LeetCode Problems:
//   - #147 Insertion Sort List [new] (inserting nodes into a sorted linked list)
//   - #21 Merge Two Sorted Lists [done] (maintaining sorted order across linked nodes)
//   - #141 Linked List Cycle [done] (cycle handling in singly linked lists)
//   - #622 Design Circular Queue [done] (circular ring buffer indexing and wrap-around)

#include "framework/lc_framework.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace {

// Circular singly linked list node matching LeetCode's definition.
struct Node {
  int val = 0;
  Node* next = nullptr;

  Node() = default;
  explicit Node(int x) : val(x) {}
  Node(int x, Node* next_node) : val(x), next(next_node) {}
};

// RAII owner for heap-allocated circular singly linked lists used in tests.
// Every node in the cycle (including nodes allocated by Solution::insert) is
// owned and uniformly destroyed when the guard leaves scope.
class CircularListGuard {
 public:
  CircularListGuard() = default;
  explicit CircularListGuard(Node* head) noexcept : head_(head) {}

  CircularListGuard(const CircularListGuard&) = delete;
  CircularListGuard& operator=(const CircularListGuard&) = delete;
  CircularListGuard(CircularListGuard&& other) noexcept
      : head_(std::exchange(other.head_, nullptr)) {}
  CircularListGuard& operator=(CircularListGuard&& other) noexcept {
    if (this != &other) {
      reset();
      head_ = std::exchange(other.head_, nullptr);
    }
    return *this;
  }

  ~CircularListGuard() { reset(); }

  // Builds a circular list from `vals` and returns an owning guard.
  [[nodiscard]] static CircularListGuard from_values(
      const std::vector<int>& vals) {
    if (vals.empty()) {
      return CircularListGuard{};
    }
    auto* head = new Node(vals.front());
    Node* tail = head;
    for (std::size_t i = 1uz; i < vals.size(); ++i) {
      tail->next = new Node(vals[i]);
      tail = tail->next;
    }
    tail->next = head;
    return CircularListGuard{head};
  }

  // Transfers ownership of the cycle head to the caller (e.g., before insert).
  [[nodiscard]] Node* release() noexcept {
    return std::exchange(head_, nullptr);
  }

  [[nodiscard]] Node* get() const noexcept { return head_; }

  // Walks one full revolution starting from `head_` and returns node values.
  [[nodiscard]] std::vector<int> to_vector() const {
    std::vector<int> result;
    if (head_ == nullptr) {
      return result;
    }
    const Node* curr = head_;
    do {
      result.push_back(curr->val);
      curr = curr->next;
    } while (curr != nullptr && curr != head_);
    return result;
  }

 private:
  void reset() noexcept {
    if (head_ == nullptr) {
      return;
    }
    // Detach head_'s outgoing link first so the cycle becomes a finite
    // nullptr-terminated chain ending at head_ itself (works for 1-node cycles
    // as well as multi-node cycles without any use-after-free).
    Node* curr = head_->next;
    head_->next = nullptr;
    while (curr != nullptr) {
      Node* next = curr->next;
      delete curr;
      curr = next;
    }
    head_ = nullptr;
  }

  Node* head_ = nullptr;
};

class Solution {
 public:
  // Inserts `insert_val` into a non-decreasing circular singly linked list and
  // returns the original `head` (or the new single-node cycle if empty).
  // Time: O(n), Space: O(1).
  Node* insert(Node* head, int insert_val) {
    // Case 1: Empty list -> create a self-referencing single-node cycle.
    if (head == nullptr) {
      auto* single = new Node(insert_val);
      single->next = single;
      return single;
    }

    Node* curr = head;
    while (true) {
      Node* next = curr->next;

      // Case 2: `insert_val` lies cleanly inside a non-decreasing adjacent pair.
      const bool fits_between =
          (curr->val <= insert_val) && (insert_val <= next->val);

      // Case 3: We are at the wrap-around cliff (max -> min), and `insert_val`
      // is either a new maximum (>= curr->val) or a new minimum (<= next->val).
      const bool fits_at_wrap =
          (curr->val > next->val) &&
          ((insert_val >= curr->val) || (insert_val <= next->val));

      if (fits_between || fits_at_wrap) {
        break;
      }

      curr = next;

      // Case 4: Full revolution completed — all nodes in the cycle have equal
      // values, so inserting after `curr` preserves sorted order.
      if (curr == head) {
        break;
      }
    }

    // Splice the new node immediately after `curr`.
    curr->next = new Node(insert_val, curr->next);
    return head;
  }
};

LC_PROBLEM(708, "Insert Into A Sorted Circular Linked List", "linked_list",
           ctx) {
  Solution sol;

  ctx.test("example_1_insert_in_middle", [&] {
    auto list = CircularListGuard::from_values({3, 4, 1});
    Node* const orig_head = list.get();
    const CircularListGuard result(sol.insert(list.release(), 2));
    ctx.expect_eq(result.get(), orig_head);
    ctx.expect_eq(result.to_vector(), std::vector<int>({3, 4, 1, 2}));
  });

  ctx.test("empty_list_creates_self_cycle", [&] {
    const CircularListGuard result(sol.insert(nullptr, 1));
    ctx.expect_true(result.get() != nullptr);
    ctx.expect_eq(result.get()->next, result.get());
    ctx.expect_eq(result.to_vector(), std::vector<int>({1}));
  });

  ctx.test("single_node_insert_smaller_and_larger", [&] {
    auto list_small = CircularListGuard::from_values({1});
    const CircularListGuard res_small(sol.insert(list_small.release(), 0));
    ctx.expect_eq(res_small.to_vector(), std::vector<int>({1, 0}));

    auto list_large = CircularListGuard::from_values({1});
    const CircularListGuard res_large(sol.insert(list_large.release(), 5));
    ctx.expect_eq(res_large.to_vector(), std::vector<int>({1, 5}));
  });

  ctx.test("insert_new_maximum_at_wrap_boundary", [&] {
    auto list = CircularListGuard::from_values({3, 4, 1});
    const CircularListGuard result(sol.insert(list.release(), 5));
    ctx.expect_eq(result.to_vector(), std::vector<int>({3, 4, 5, 1}));
  });

  ctx.test("insert_new_minimum_at_wrap_boundary", [&] {
    auto list = CircularListGuard::from_values({3, 4, 1});
    const CircularListGuard result(sol.insert(list.release(), 0));
    ctx.expect_eq(result.to_vector(), std::vector<int>({3, 4, 0, 1}));
  });

  ctx.test("all_equal_values_terminates_cleanly", [&] {
    auto list = CircularListGuard::from_values({2, 2, 2, 2});
    Node* const orig_head = list.get();
    const CircularListGuard result(sol.insert(list.release(), 3));
    ctx.expect_eq(result.get(), orig_head);
    ctx.expect_eq(result.to_vector(), std::vector<int>({2, 3, 2, 2, 2}));
  });

  ctx.test("duplicate_value_matches_existing_node", [&] {
    auto list = CircularListGuard::from_values({3, 5, 1});
    const CircularListGuard result(sol.insert(list.release(), 3));
    ctx.expect_eq(result.to_vector(), std::vector<int>({3, 3, 5, 1}));
  });
}

}  // namespace
