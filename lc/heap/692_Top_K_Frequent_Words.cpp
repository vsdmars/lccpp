// LeetCode 692: Top K Frequent Words
// https://leetcode.com/problems/top-k-frequent-words/
//
// Problem:
//   Given an array of strings `words` and an integer `k`, return the `k` most
//   frequent strings ordered from highest frequency to lowest frequency. Words
//   with the same frequency must be ordered lexicographically (ascending).
//
// Algorithm:
//   1. Bounded Min-Heap of Size k (primary):
//      Count each word's frequency in a hash map keyed by `std::string_view`
//      (zero extra string copies during counting). Maintain a priority queue
//      of at most k `(word, count)` entries ordered so the *worst* candidate
//      (lowest frequency; on ties, lexicographically largest word) sits on
//      top. Whenever the heap exceeds size k, pop the top element. Finally,
//      pop the remaining k entries and reverse them to produce best-to-worst
//      order.
//      Time:  O(N * L + U * L * log(k)), where N = words.size(), U = unique
//             words, and L = average word length.
//      Space: O(U)
//
//   2. Frequency Map + Full Sort (alternative matching Python `Counter` sort):
//      Count frequencies, collect unique `(word, count)` pairs, sort by
//      descending count then ascending word, and take the first k words.
//      Time:  O(N * L + U * L * log(U))
//      Space: O(U)
//
// Data Structures:
//   - Hash map (`std::unordered_map<std::string_view, int>`) for O(1) average
//     frequency counting without duplicating input strings.
//   - Bounded priority queue (`std::priority_queue`) with custom tie-breaking
//     comparator keeping only the top k candidates in memory.
//
// Real-World Applications:
//   - Search engine autocomplete and trending query dashboards.
//   - Log analytics and observability systems surfacing top-k recurring error
//     messages or endpoints.
//   - NLP corpus analysis and TF-IDF vocabulary pruning.
//
// Related LeetCode Problems:
//   - #347 Top K Frequent Elements [done] (integer counterpart without lexical tie-break)
//   - #451 Sort Characters By Frequency [done] (frequency-ordered string reconstruction)
//   - #973 K Closest Points to Origin [done] (bounded heap top-k selection)
//   - #215 Kth Largest Element in an Array [done] (order statistics / heap selection)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

class Solution {
 public:
  // Returns the k most frequent words (highest frequency first, ties broken
  // lexicographically) using a bounded size-k heap.
  // Time: O(N * L + U * L * log(k)), Space: O(U).
  std::vector<std::string> topKFrequent(const std::vector<std::string>& words,
                                        int k) {
    if (words.empty() || k <= 0) {
      return {};
    }

    // Borrow string_views into `words` to avoid copying strings while counting.
    std::unordered_map<std::string_view, int> counts;
    counts.reserve(words.size());
    for (const auto& word : words) {
      ++counts[word];
    }

    struct WordCount {
      std::string_view word;
      int count = 0;
    };

    // Comparator that places the *worst* word (lowest frequency, or
    // lexicographically largest on ties) at `top()` so it is evicted first.
    struct WorseAtTop {
      [[nodiscard]] bool operator()(const WordCount& lhs,
                                    const WordCount& rhs) const noexcept {
        if (lhs.count != rhs.count) {
          return lhs.count > rhs.count;
        }
        return lhs.word < rhs.word;
      }
    };

    const auto limit = static_cast<std::size_t>(k);
    std::priority_queue<WordCount, std::vector<WordCount>, WorseAtTop> heap;

    for (const auto& [word, count] : counts) {
      heap.push(WordCount{.word = word, .count = count});
      if (heap.size() > limit) {
        heap.pop();
      }
    }

    // Popping yields worst-to-best; reverse once to return best-to-worst.
    std::vector<std::string> result;
    result.reserve(heap.size());
    while (!heap.empty()) {
      result.emplace_back(heap.top().word);
      heap.pop();
    }
    std::ranges::reverse(result);
    return result;
  }

  // Alternative sorting implementation mirroring the Python Counter + sort
  // solution. Time: O(N * L + U * L * log(U)), Space: O(U).
  std::vector<std::string> topKFrequentSort(
      const std::vector<std::string>& words, int k) {
    if (words.empty() || k <= 0) {
      return {};
    }

    std::unordered_map<std::string_view, int> counts;
    counts.reserve(words.size());
    for (const auto& word : words) {
      ++counts[word];
    }

    std::vector<std::string_view> unique_words;
    unique_words.reserve(counts.size());
    for (const auto& entry : counts) {
      unique_words.push_back(entry.first);
    }

    std::ranges::sort(unique_words,
                      [&](std::string_view lhs, std::string_view rhs) {
                        const int lc = counts.at(lhs);
                        const int rc = counts.at(rhs);
                        if (lc != rc) {
                          return lc > rc;
                        }
                        return lhs < rhs;
                      });

    const std::size_t take =
        std::min(unique_words.size(), static_cast<std::size_t>(k));
    std::vector<std::string> result;
    result.reserve(take);
    for (std::size_t i = 0uz; i < take; ++i) {
      result.emplace_back(unique_words[i]);
    }
    return result;
  }
};

LC_PROBLEM(692, "Top K Frequent Words", "heap", ctx) {
  ctx.test("example_1_tie_break", [&] {
    Solution sol;
    const std::vector<std::string> words = {"i", "love", "leetcode",
                                            "i", "love", "coding"};
    const std::vector<std::string> expected = {"i", "love"};
    ctx.expect_eq(sol.topKFrequent(words, 2), expected);
    ctx.expect_eq(sol.topKFrequentSort(words, 2), expected);
  });

  ctx.test("example_2_distinct_frequencies", [&] {
    Solution sol;
    const std::vector<std::string> words = {"the", "day",   "is", "sunny",
                                            "the", "the",   "the", "sunny",
                                            "is",  "is"};
    const std::vector<std::string> expected = {"the", "is", "sunny", "day"};
    ctx.expect_eq(sol.topKFrequent(words, 4), expected);
    ctx.expect_eq(sol.topKFrequentSort(words, 4), expected);
  });

  ctx.test("all_equal_frequency_alphabetical", [&] {
    Solution sol;
    const std::vector<std::string> words = {"delta", "beta", "alpha", "gamma"};
    const std::vector<std::string> expected = {"alpha", "beta"};
    ctx.expect_eq(sol.topKFrequent(words, 2), expected);
    ctx.expect_eq(sol.topKFrequentSort(words, 2), expected);
  });

  ctx.test("single_word_and_zero_k", [&] {
    Solution sol;
    const std::vector<std::string> words = {"solo", "solo"};
    ctx.expect_eq(sol.topKFrequent(words, 1), std::vector<std::string>{"solo"});
    ctx.expect_eq(sol.topKFrequentSort(words, 1),
                  std::vector<std::string>{"solo"});
    ctx.expect_eq(sol.topKFrequent(words, 0), std::vector<std::string>{});
  });
}

}  // namespace
