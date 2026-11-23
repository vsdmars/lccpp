// LeetCode 737: Sentence Similarity II
// https://leetcode.com/problems/sentence-similarity-ii/
//
// Problem:
//   Given two sentences `sentence1` and `sentence2` (each an array of words)
//   and a list of similar word pairs `similarPairs`, determine whether the two
//   sentences are similar. Two sentences are similar if they have the same
//   length and every pair of words at the same index `sentence1[i]` and
//   `sentence2[i]` is similar. Word similarity is reflexive, symmetric, and
//   transitive (i.e., an equivalence relation).
//
// Examples:
//   Example 1:
//     Input:  sentence1 = ["great", "acting", "skills"], sentence2 = ["fine",
//             "drama", "talent"], similarPairs = [["great", "good"], ["fine",
//             "good"], ["acting", "drama"], ["skills", "talent"]]
//     Output: true
//     Explanation:
//       - Verify sentence1.size() == sentence2.size() (3 == 3), then insert all
//         four pairs into WordDisjointSet via dsu.unite.
//       - Uniting ["great", "good"] and ["fine", "good"] places "great", "good",
//         and "fine" in one connected component, while ["acting", "drama"] and
//         ["skills", "talent"] form two more components.
//       - Check each index i from 0 to 2: dsu.connected returns true for ("great",
//         "fine"), ("acting", "drama"), and ("skills", "talent") because each pair
//         shares a DSU root, so return true.
//
//   Example 2:
//     Input:  sentence1 = ["great", "acting", "skills"], sentence2 = ["fine",
//             "drama", "talent"], similarPairs = [["great", "good"], ["it",
//             "good"], ["acting", "drama"], ["skills", "talent"]]
//     Output: false
//     Explanation:
//       - Verify both sentences have length 3 and populate WordDisjointSet with
//         components {"great", "good", "it"}, {"acting", "drama"}, and {"skills",
//         "talent"}.
//       - At i = 0, dsu.connected("great", "fine") sees "great" != "fine" and looks
//         up both words in word_to_id_.
//       - Because "fine" is absent from word_to_id_, connected returns false and
//         areSentencesSimilarTwo immediately returns false.
//
//   Example 3:
//     Input:  sentence1 = ["great"], sentence2 = ["great"], similarPairs = []
//     Output: true
//     Explanation:
//       - Both sentences have size 1, and iterating over the empty similarPairs
//         list leaves WordDisjointSet empty.
//       - At i = 0, dsu.connected("great", "great") hits the lhs == rhs check first
//         and returns true by reflexivity, yielding true.
//
// Algorithm:
//   1. Return `false` immediately if `sentence1.size() != sentence2.size()`.
//   2. Insert every pair `(u, v)` from `similar_pairs` into a Union-Find
//      structure (`WordDisjointSet`) that merges their equivalence classes using
//      union by rank and iterative path halving.
//   3. For each index `i`, check whether `sentence1[i]` and `sentence2[i]` are
//      connected: identical words match immediately (reflexivity); otherwise,
//      both words must exist in the DSU and share the same representative root.
//   Key Invariant:
//     Two distinct words are transitively similar if and only if they belong to
//     the same connected component in the similarity graph.
//   Time Complexity:
//     O((P + N) * L + (P + N) * alpha(P)), where P = `similar_pairs.size()`,
//     N = `sentence1.size()`, L = average word length, and alpha is the inverse
//     Ackermann function.
//   Space Complexity:
//     O(P) for the hash map and DSU arrays; `std::string_view` keys reference
//     the input strings directly without allocating string copies.
//
// Data Structures:
//   - `std::unordered_map<std::string_view, std::size_t>` (`word_to_id_`):
//     Assigns each unique word from `similar_pairs` a dense integer ID without
//     copying string data.
//   - `std::vector<std::size_t>` (`parent_`) and `std::vector<int>` (`rank_`):
//     Array-backed Disjoint Set Union (Union-Find) storing parent pointers and
//     tree ranks for near-O(1) amortized `find` and `unite` operations.
//
// Real-World Applications:
//   - Search query expansion and synonym canonicalization in information
//     retrieval and NLP pipelines.
//   - Entity resolution and record deduplication (merging alias tables across
//     databases).
//   - Compiler type-alias and symbol equivalence checking.
//
// Related LeetCode Problems:
//   - #734 Sentence Similarity [done] (non-transitive version solvable with a hash
//     set)
//   - #721 Accounts Merge [done] (grouping string entities transitively via
//     Union-Find)
//   - #990 Satisfiability of Equality Equations [new] (building transitive
//     equivalence classes with Union-Find)
//   - #1061 Lexicographically Smallest Equivalent String [new] (character equivalence
//     classes via Union-Find)
//   - #547 Number of Provinces [done] (foundational connected components with
//     Union-Find)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

class WordDisjointSet {
 public:
  std::size_t get_or_add(std::string_view word) {
    auto [it, inserted] = word_to_id_.emplace(word, parent_.size());
    if (inserted) {
      parent_.push_back(it->second);
      rank_.push_back(0);
    }
    return it->second;
  }

  void unite(std::string_view lhs, std::string_view rhs) {
    std::size_t root_a = find(get_or_add(lhs));
    std::size_t root_b = find(get_or_add(rhs));
    if (root_a == root_b) {
      return;
    }
    if (rank_[root_a] < rank_[root_b]) {
      std::swap(root_a, root_b);
    }
    parent_[root_b] = root_a;
    if (rank_[root_a] == rank_[root_b]) {
      ++rank_[root_a];
    }
  }

  bool connected(std::string_view lhs, std::string_view rhs) {
    if (lhs == rhs) {
      return true;
    }
    const auto it_a = word_to_id_.find(lhs);
    const auto it_b = word_to_id_.find(rhs);
    if (it_a == word_to_id_.end() || it_b == word_to_id_.end()) {
      return false;
    }
    return find(it_a->second) == find(it_b->second);
  }

 private:
  std::size_t find(std::size_t x) {
    while (parent_[x] != x) {
      parent_[x] = parent_[parent_[x]];
      x = parent_[x];
    }
    return x;
  }

  std::unordered_map<std::string_view, std::size_t> word_to_id_;
  std::vector<std::size_t> parent_;
  std::vector<int> rank_;
};

class Solution {
 public:
  // Build equivalence classes over all words appearing in `similar_pairs` using
  // Union-Find, then verify that every corresponding pair of words either
  // matches verbatim or belongs to the same connected component.
  // Time: O((P + N) * L + (P + N) * alpha(P)), where P = similar_pairs.size(),
  //       N = sentence1.size(), L = average word length.
  // Space: O(P) using non-owning string_view keys into `similar_pairs`.
  bool areSentencesSimilarTwo(
      const std::vector<std::string>& sentence1,
      const std::vector<std::string>& sentence2,
      const std::vector<std::vector<std::string>>& similar_pairs) {
    if (sentence1.size() != sentence2.size()) {
      return false;
    }

    WordDisjointSet dsu;
    for (const auto& pair : similar_pairs) {
      dsu.unite(pair[0], pair[1]);
    }

    for (std::size_t i = 0uz; i < sentence1.size(); ++i) {
      if (!dsu.connected(sentence1[i], sentence2[i])) {
        return false;
      }
    }
    return true;
  }
};

LC_PROBLEM(737, "Sentence Similarity II", "union_find", ctx) {
  ctx.test("transitive_similar_true", [&] {
    Solution sol;
    const std::vector<std::string> s1 = {"great", "acting", "skills"};
    const std::vector<std::string> s2 = {"fine", "drama", "talent"};
    const std::vector<std::vector<std::string>> pairs = {{"great", "good"},
                                                         {"fine", "good"},
                                                         {"acting", "drama"},
                                                         {"skills", "talent"}};
    ctx.expect_true(sol.areSentencesSimilarTwo(s1, s2, pairs));
  });

  ctx.test("disconnected_words_false", [&] {
    Solution sol;
    const std::vector<std::string> s1 = {"great", "acting", "skills"};
    const std::vector<std::string> s2 = {"fine", "drama", "talent"};
    const std::vector<std::vector<std::string>> pairs = {{"great", "good"},
                                                         {"it", "good"},
                                                         {"acting", "drama"},
                                                         {"skills", "talent"}};
    ctx.expect_false(sol.areSentencesSimilarTwo(s1, s2, pairs));
  });

  ctx.test("identical_without_pairs", [&] {
    Solution sol;
    const std::vector<std::string> s1 = {"great"};
    const std::vector<std::string> s2 = {"great"};
    const std::vector<std::vector<std::string>> pairs = {};
    ctx.expect_true(sol.areSentencesSimilarTwo(s1, s2, pairs));
  });

  ctx.test("different_lengths_false", [&] {
    Solution sol;
    const std::vector<std::string> s1 = {"great"};
    const std::vector<std::string> s2 = {"doubleplus", "good"};
    const std::vector<std::vector<std::string>> pairs = {};
    ctx.expect_false(sol.areSentencesSimilarTwo(s1, s2, pairs));
  });

  ctx.test("long_sentence_from_build", [&] {
    Solution sol;
    const std::vector<std::string> s1 = {
        "this",      "summer",        "thomas", "get",    "really",
        "very",      "rich",          "and",    "have",   "any",
        "actually",  "wonderful",     "and",    "good",   "truck",
        "every",     "morning",       "he",     "drives", "an",
        "extraordinary", "truck",     "around", "the",    "nice",
        "city",      "to",            "eat",    "some",   "extremely",
        "extraordinary", "food",      "as",     "his",    "meal",
        "but",       "he",            "only",   "entertain", "an",
        "few",       "well",          "fruits", "as",     "single",
        "lunch",     "he",            "wants",  "to",     "eat",
        "single",    "single",        "and",    "really", "healthy",
        "life"};
    const std::vector<std::string> s2 = {
        "this",      "summer",        "thomas", "get",    "very",
        "extremely", "rich",          "and",    "possess", "the",
        "actually",  "great",         "and",    "wonderful", "vehicle",
        "every",     "morning",       "he",     "drives", "unique",
        "extraordinary", "automobile", "around", "unique", "fine",
        "city",      "to",            "drink",  "single", "extremely",
        "nice",      "meal",          "as",     "his",    "super",
        "but",       "he",            "only",   "entertain", "a",
        "few",       "extraordinary", "food",   "as",     "some",
        "brunch",    "he",            "wants",  "to",     "take",
        "any",       "some",          "and",    "really", "healthy",
        "life"};
    const std::vector<std::vector<std::string>> pairs = {
        {"good", "nice"},         {"good", "excellent"},
        {"good", "well"},         {"good", "great"},
        {"fine", "nice"},         {"fine", "excellent"},
        {"fine", "well"},         {"fine", "great"},
        {"wonderful", "nice"},    {"wonderful", "excellent"},
        {"wonderful", "well"},    {"wonderful", "great"},
        {"extraordinary", "nice"}, {"extraordinary", "excellent"},
        {"extraordinary", "well"}, {"extraordinary", "great"},
        {"one", "a"},             {"one", "an"},
        {"one", "unique"},        {"one", "any"},
        {"single", "a"},          {"single", "an"},
        {"single", "unique"},     {"single", "any"},
        {"the", "a"},             {"the", "an"},
        {"the", "unique"},        {"the", "any"},
        {"some", "a"},            {"some", "an"},
        {"some", "unique"},       {"some", "any"},
        {"car", "vehicle"},       {"car", "automobile"},
        {"car", "truck"},         {"auto", "vehicle"},
        {"auto", "automobile"},   {"auto", "truck"},
        {"wagon", "vehicle"},     {"wagon", "automobile"},
        {"wagon", "truck"},       {"have", "take"},
        {"have", "drink"},        {"eat", "take"},
        {"eat", "drink"},         {"entertain", "take"},
        {"entertain", "drink"},   {"meal", "lunch"},
        {"meal", "dinner"},       {"meal", "breakfast"},
        {"meal", "fruits"},       {"super", "lunch"},
        {"super", "dinner"},      {"super", "breakfast"},
        {"super", "fruits"},      {"food", "lunch"},
        {"food", "dinner"},       {"food", "breakfast"},
        {"food", "fruits"},       {"brunch", "lunch"},
        {"brunch", "dinner"},     {"brunch", "breakfast"},
        {"brunch", "fruits"},     {"own", "have"},
        {"own", "possess"},       {"keep", "have"},
        {"keep", "possess"},      {"very", "super"},
        {"very", "actually"},     {"really", "super"},
        {"really", "actually"},   {"extremely", "super"},
        {"extremely", "actually"}};
    ctx.expect_true(sol.areSentencesSimilarTwo(s1, s2, pairs));
  });
}

}  // namespace
