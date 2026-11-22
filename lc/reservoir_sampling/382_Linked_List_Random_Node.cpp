// https://leetcode.com/problems/linked-list-random-node/
//
// Given a singly linked list, return a random node's value from the linked list
// such that each node has the same probability of being chosen, using O(1)
// extra space without knowing the list length in advance (Reservoir Sampling).

#include "framework/lc_framework.h"

#include <cstdint>
#include <deque>
#include <initializer_list>
#include <random>
#include <unordered_map>

namespace {

struct ListNode {
  int val = 0;
  ListNode* next = nullptr;

  explicit ListNode(int x, ListNode* next_node = nullptr)
      : val(x), next(next_node) {}
};

// RAII arena that owns all ListNodes constructed during a test case.
struct ListArena {
  std::deque<ListNode> nodes;

  ListNode* build(std::initializer_list<int> values) {
    ListNode sentinel(0);
    ListNode* tail = &sentinel;
    for (int v : values) {
      nodes.emplace_back(v);
      tail->next = &nodes.back();
      tail = tail->next;
    }
    return sentinel.next;
  }
};

// Reservoir sampling for k = 1: when visiting the i-th node (1-indexed),
// replace the currently chosen value with probability 1 / i. By induction,
// after visiting all n nodes, every node is chosen with probability 1 / n.
//
// Time:  O(1) construction, O(n) per getRandom()
// Space: O(1) auxiliary
class Solution {
 public:
  explicit Solution(ListNode* head, std::uint32_t seed = std::random_device{}())
      : head_(head), rng_(seed) {}

  int getRandom() {
    int chosen = 0;
    int seen = 0;
    for (const ListNode* curr = head_; curr != nullptr; curr = curr->next) {
      ++seen;
      std::uniform_int_distribution<int> dist(0, seen - 1);
      if (dist(rng_) == 0) {
        chosen = curr->val;
      }
    }
    return chosen;
  }

 private:
  ListNode* head_ = nullptr;
  std::mt19937 rng_;
};

LC_PROBLEM(382, "Linked List Random Node", "reservoir_sampling", ctx) {
  ctx.test("single_node_list", [&] {
    ListArena arena;
    ListNode* head = arena.build({42});
    Solution sol(head, 7u);
    for (int i = 0; i < 50; ++i) {
      ctx.expect_eq(sol.getRandom(), 42);
    }
  });

  ctx.test("example_uniform_distribution", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3});
    Solution sol(head, 42u);

    std::unordered_map<int, int> freq;
    constexpr int kTrials = 6000;
    for (int i = 0; i < kTrials; ++i) {
      const int val = sol.getRandom();
      ctx.expect_true(val == 1 || val == 2 || val == 3);
      ++freq[val];
    }

    for (int expected_val : {1, 2, 3}) {
      const int count = freq[expected_val];
      ctx.expect_true(count >= 1800 && count <= 2200,
                      "expected roughly uniform count around 2000");
    }
  });

  ctx.test("build_list_uniform_distribution", [&] {
    ListArena arena;
    ListNode* head = arena.build({1, 2, 3, 7, 8});
    Solution sol(head, 12345u);

    std::unordered_map<int, int> freq;
    constexpr int kTrials = 10000;
    for (int i = 0; i < kTrials; ++i) {
      ++freq[sol.getRandom()];
    }

    ctx.expect_eq(freq.size(), 5uz);
    for (int expected_val : {1, 2, 3, 7, 8}) {
      const int count = freq[expected_val];
      ctx.expect_true(count >= 1800 && count <= 2200,
                      "expected roughly uniform count around 2000");
    }
  });

  ctx.test("default_seeded_constructor_and_negatives", [&] {
    ListArena arena;
    ListNode* head = arena.build({-10, 0, 10});
    Solution sol(head);
    for (int i = 0; i < 30; ++i) {
      const int val = sol.getRandom();
      ctx.expect_true(val == -10 || val == 0 || val == 10);
    }
  });
}

}  // namespace
