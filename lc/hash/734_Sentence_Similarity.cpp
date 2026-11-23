// LeetCode 734: Sentence Similarity
// https://leetcode.com/problems/sentence-similarity/
//
// Problem:
//   Given two sentences `sentence1` and `sentence2` (arrays of words) and a
//   list of similar word pairs `similarPairs`, determine whether the two
//   sentences are similar. Two sentences are similar iff:
//     1. They have the exact same number of words.
//     2. For every index i, sentence1[i] and sentence2[i] are either identical
//        (reflexivity) or appear together in `similarPairs` in either order
//        (symmetry). Similarity is *not* transitive.
//   Constraints: 1 <= sentence1.length, sentence2.length <= 1000,
//   0 <= similarPairs.length <= 2000.
//
// Examples:
//   Example 1:
//     Input:  sentence1 = ["great", "acting", "skills"], sentence2 = ["fine",
//             "drama", "talent"], similarPairs = [["great", "fine"], ["acting",
//             "drama"], ["skills", "talent"]]
//     Output: true
//     Explanation:
//       - Verify sentence1.size() == sentence2.size() (3 == 3) and populate the
//         symmetric hash map similar with bidirectional entries for each pair.
//       - At i = 0, "great" != "fine", so look up similar["great"] and confirm it
//         contains "fine".
//       - At i = 1 and i = 2, verify similar["acting"] contains "drama" and
//         similar["skills"] contains "talent".
//       - All three positions match, so return true.
//
//   Example 2:
//     Input:  sentence1 = ["great"], sentence2 = ["good"], similarPairs =
//             [["great", "fine"], ["fine", "good"]]
//     Output: false
//     Explanation:
//       - Both sentences have size 1, so build similar with direct edges great <->
//         fine and fine <-> good.
//       - At i = 0, "great" != "good", so look up similar["great"], whose set is
//         {"fine"}.
//       - Because "good" is absent from similar["great"] (similarity is not
//         transitive), immediately return false.
//
//   Example 3:
//     Input:  sentence1 = ["fine"], sentence2 = ["great"], similarPairs =
//             [["great", "fine"]]
//     Output: true
//     Explanation:
//       - Verify sentence1.size() == sentence2.size() (1 == 1) and insert both
//         directions of ["great", "fine"] into similar so similar["fine"] contains
//         "great".
//       - At i = 0, "fine" != "great", so look up similar["fine"] and find "great"
//         present due to symmetric insertion.
//       - Complete the loop with no mismatches and return true.
//
// Algorithm:
//   1. Immediately reject if `sentence1.size() != sentence2.size()`.
//   2. Build a symmetric adjacency hash map mapping each word to the hash set
//      of words directly paired with it in `similarPairs`. Using
//      `std::string_view` keys referencing `similarPairs` avoids heap copies.
//   3. Scan indices `i = 0 .. n - 1`:
//        - If `sentence1[i] == sentence2[i]`, continue (reflexive match).
//        - Otherwise verify that `sentence1[i]` is present in the map and its
//          neighbor set contains `sentence2[i]`. Return false on any failure.
//   4. Return true if all positions match.
//   Time Complexity: O(P * L + N * L), where P = similarPairs.size(),
//   N = sentence1.size(), and L = average word length (for hashing/comparing).
//   Space Complexity: O(P) hash-table entries (zero string duplication via
//   `std::string_view`).
//
// Data Structures:
//   - Hash map of hash sets
//     (`std::unordered_map<std::string_view, std::unordered_set<std::string_view>>`):
//     encodes the symmetric, non-transitive similarity relation for O(1)
//     average lookup per word pair.
//
// Real-World Applications:
//   - Search query expansion and synonym-aware query equivalence checking in
//     information retrieval systems.
//   - Natural-language test assertions and paraphrase normalization where
//     curated 1-hop synonym tables define permissible token substitutions.
//   - Schema/column-name reconciliation in ETL pipelines using alias tables.
//
// Related LeetCode Problems:
//   - #737 Sentence Similarity II [done] (transitive extension solved with Union-Find / DFS)
//   - #205 Isomorphic Strings [done] (position-wise character mapping validation)
//   - #290 Word Pattern [done] (bijective token correspondence between sequences)
//   - #547 Number of Provinces [done] (graph connectivity / equivalence classes)

#include "framework/lc_framework.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

class Solution {
 public:
  // Determines whether `sentence1` and `sentence2` are element-wise similar
  // under the reflexive, symmetric, non-transitive relation `similar_pairs`.
  // Time: O((P + N) * L), Space: O(P).
  bool areSentencesSimilar(
      const std::vector<std::string>& sentence1,
      const std::vector<std::string>& sentence2,
      const std::vector<std::vector<std::string>>& similar_pairs) {
    // Sentences of different word counts can never be similar.
    if (sentence1.size() != sentence2.size()) {
      return false;
    }

    // Build symmetric 1-hop lookup table using non-owning views into
    // `similar_pairs` (which remains alive for the full call).
    std::unordered_map<std::string_view, std::unordered_set<std::string_view>>
        similar;
    similar.reserve(similar_pairs.size() * 2uz);
    for (const auto& pair : similar_pairs) {
      similar[pair[0]].insert(pair[1]);
      similar[pair[1]].insert(pair[0]);
    }

    // Verify each corresponding word pair satisfies reflexivity or direct edge.
    for (std::size_t i = 0uz; i < sentence1.size(); ++i) {
      if (sentence1[i] == sentence2[i]) {
        continue;
      }

      const auto it = similar.find(sentence1[i]);
      if (it == similar.end() || !it->second.contains(sentence2[i])) {
        return false;
      }
    }

    return true;
  }
};

LC_PROBLEM(734, "Sentence Similarity", "hash", ctx) {
  Solution sol;

  ctx.test("leetcode_example_1", [&] {
    const std::vector<std::string> s1 = {"great", "acting", "skills"};
    const std::vector<std::string> s2 = {"fine", "drama", "talent"};
    const std::vector<std::vector<std::string>> pairs = {
        {"great", "fine"}, {"acting", "drama"}, {"skills", "talent"}};
    ctx.expect_true(sol.areSentencesSimilar(s1, s2, pairs));
  });

  ctx.test("large_synonym_table_from_build", [&] {
    const std::vector<std::string> s1 = {"an", "extraordinary", "meal"};
    const std::vector<std::string> s2 = {"one", "good", "dinner"};
    const std::vector<std::vector<std::string>> pairs = {
        {"great", "good"},         {"extraordinary", "good"},
        {"well", "good"},          {"wonderful", "good"},
        {"excellent", "good"},     {"fine", "good"},
        {"nice", "good"},          {"any", "one"},
        {"some", "one"},           {"unique", "one"},
        {"the", "one"},            {"an", "one"},
        {"single", "one"},         {"a", "one"},
        {"truck", "car"},          {"wagon", "car"},
        {"automobile", "car"},     {"auto", "car"},
        {"vehicle", "car"},        {"entertain", "have"},
        {"drink", "have"},         {"eat", "have"},
        {"take", "have"},          {"fruits", "meal"},
        {"brunch", "meal"},        {"breakfast", "meal"},
        {"food", "meal"},          {"dinner", "meal"},
        {"super", "meal"},         {"lunch", "meal"},
        {"possess", "own"},        {"keep", "own"},
        {"have", "own"},           {"extremely", "very"},
        {"actually", "very"},      {"really", "very"},
        {"super", "very"}};
    ctx.expect_true(sol.areSentencesSimilar(s1, s2, pairs));
  });

  ctx.test("not_transitive", [&] {
    const std::vector<std::string> s1 = {"great"};
    const std::vector<std::string> s2 = {"good"};
    const std::vector<std::vector<std::string>> pairs = {{"great", "fine"},
                                                         {"fine", "good"}};
    ctx.expect_false(sol.areSentencesSimilar(s1, s2, pairs));
  });

  ctx.test("reflexive_and_different_length", [&] {
    ctx.expect_true(sol.areSentencesSimilar({"great"}, {"great"}, {}));
    ctx.expect_false(sol.areSentencesSimilar(
        {"great"}, {"doubleplus", "good"}, {{"great", "doubleplus"}}));
  });

  ctx.test("symmetry_reversed_pair", [&] {
    ctx.expect_true(
        sol.areSentencesSimilar({"fine"}, {"great"}, {{"great", "fine"}}));
  });
}

}  // namespace
