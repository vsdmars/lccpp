// LeetCode 843: Guess The Word
// https://leetcode.com/problems/guess-the-word/
//
// Problem:
//   This is an interactive problem. Given a list of unique 6-letter lowercase
//   words where one word is chosen as the hidden `secret`, call
//   `master.guess(word)` to learn how many positions match `secret` in both
//   character and index (0..6, or -1 if `word` is not in the original list).
//   Identify `secret` within the allowed guess budget (typically 10 guesses).
//
// Algorithm:
//   Use deterministic 1-step Minimax candidate elimination (generalizing the
//   0-match heuristic to all 6 non-winning match buckets):
//     1. Start with `candidates` containing all words from `words`.
//     2. At each round, evaluate every candidate `w` by partitioning all other
//        candidates `u != w` into 6 buckets according to `countMatches(w, u)`
//        (match counts 0..5; match count 6 means `w` itself is the secret and
//        wins immediately). The worst-case remaining pool if `w` is guessed is
//        `max_{m in 0..5} bucket[m]`.
//     3. Choose the candidate `best` that minimizes this worst-case remaining
//        pool size (minimax), guaranteeing maximal information gain even on
//        adversarial or clustered wordlists where few pairs have 0 matches.
//     4. Query `matches = master.guess(best)`. If `matches == 6`, terminate.
//        Otherwise filter `candidates` down to words `w` satisfying
//        `countMatches(w, best) == matches`, because the true secret must share
//        exact match count `matches` with `best`.
//   Time Complexity:  O(G * N^2 * L) worst-case across G <= 10 guesses, where
//                     N is the candidate count (shrinking geometrically each
//                     round) and L = 6 is the word length.
//   Space Complexity: O(N * L) for the active candidate pool.
//
// Data Structures:
//   - `std::vector<std::string>` for the shrinking set of valid candidates.
//   - Fixed-size `std::array<int, 7>` histogram per candidate for O(1)-space
//     bucket counting during minimax selection.
//   - `std::unordered_set<std::string>` inside the mock `Master` judge to
//     validate guesses in O(1) expected time.
//
// Real-World Applications:
//   - Active learning and optimal experimental design (selecting the next query
//     that minimizes worst-case hypothesis space).
//   - Fault localization and automated diagnostic probing in distributed
//     systems (selecting tests that split candidate root causes evenly).
//   - Cryptanalysis / code-breaking games (Knuth's five-guess Mastermind
//     minimax algorithm) and Wordle solvers.
//
// Related LeetCode Problems:
//   - #374 Guess Number Higher or Lower [done] (interactive binary-search oracle)
//   - #375 Guess Number Higher or Lower II [done] (minimax DP over guess costs)
//   - #299 Bulls and Cows [done] (exact positional match counting)
//   - #278 First Bad Version [done] (minimizing interactive oracle calls)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

constexpr std::size_t kWordLength = 6uz;
constexpr int kWordLengthInt = 6;
constexpr int kDefaultAllowedGuesses = 10;

// Mock interactive judge implementing LeetCode's Master API.
class Master {
 public:
  Master(std::string secret, const std::vector<std::string>& wordlist,
         int allowed_guesses = kDefaultAllowedGuesses)
      : secret_(std::move(secret)),
        wordlist_(wordlist.begin(), wordlist.end()),
        allowed_guesses_(allowed_guesses) {}

  // Returns the number of exact positional matches with `secret_`, or -1 if
  // `word` is not in the original wordlist.
  int guess(const std::string& word) {
    ++guesses_made_;
    if (!wordlist_.contains(word)) {
      return -1;
    }
    int matches = 0;
    for (std::size_t i = 0uz; i < kWordLength; ++i) {
      if (word[i] == secret_[i]) {
        ++matches;
      }
    }
    if (matches == kWordLengthInt) {
      found_ = true;
    }
    return matches;
  }

  [[nodiscard]] bool found() const { return found_; }
  [[nodiscard]] int guessesMade() const { return guesses_made_; }
  [[nodiscard]] bool passed() const {
    return found_ && guesses_made_ <= allowed_guesses_;
  }

 private:
  std::string secret_;
  std::unordered_set<std::string> wordlist_;
  int allowed_guesses_ = kDefaultAllowedGuesses;
  int guesses_made_ = 0;
  bool found_ = false;
};

class Solution {
 public:
  // Finds the hidden secret word via deterministic minimax candidate pruning.
  // Time: O(G * N^2 * L), Space: O(N * L).
  void findSecretWord(const std::vector<std::string>& words, Master& master) {
    std::vector<std::string> candidates = words;

    while (!candidates.empty()) {
      const std::string guess_word = selectMinimaxCandidate(candidates);
      const int matches = master.guess(guess_word);
      if (matches == kWordLengthInt) {
        return;
      }

      // Retain only candidates consistent with the judge's feedback: the true
      // secret must have exactly `matches` positions in common with guess_word.
      std::vector<std::string> next_candidates;
      next_candidates.reserve(candidates.size());
      for (const std::string& word : candidates) {
        if (word != guess_word && countMatches(word, guess_word) == matches) {
          next_candidates.push_back(word);
        }
      }
      candidates = std::move(next_candidates);
    }
  }

 private:
  // Counts exact positional character matches between two 6-letter words.
  static int countMatches(std::string_view a, std::string_view b) {
    int matches = 0;
    for (std::size_t i = 0uz; i < kWordLength; ++i) {
      if (a[i] == b[i]) {
        ++matches;
      }
    }
    return matches;
  }

  // Picks the candidate whose worst-case remaining bucket size (over all
  // non-winning match outcomes 0..5) is minimized.
  static std::string selectMinimaxCandidate(
      const std::vector<std::string>& candidates) {
    std::size_t best_idx = 0uz;
    int best_worst_case = std::numeric_limits<int>::max();

    for (std::size_t i = 0uz; i < candidates.size(); ++i) {
      std::array<int, kWordLength> buckets{};
      for (std::size_t j = 0uz; j < candidates.size(); ++j) {
        if (i == j) {
          continue;
        }
        const int m = countMatches(candidates[i], candidates[j]);
        if (m < kWordLengthInt) {
          ++buckets[static_cast<std::size_t>(m)];
        }
      }

      const int worst_case = *std::ranges::max_element(buckets);
      if (worst_case < best_worst_case) {
        best_worst_case = worst_case;
        best_idx = i;
      }
    }

    return candidates[best_idx];
  }
};

// Generates a deterministic list of unique random 6-letter lowercase words.
std::vector<std::string> makeRandomWordlist(std::size_t count,
                                            std::uint32_t seed) {
  std::mt19937 rng(seed);
  std::uniform_int_distribution<int> dist(0, 25);
  std::unordered_set<std::string> seen;
  std::vector<std::string> words;
  words.reserve(count);

  while (words.size() < count) {
    std::string word(kWordLength, 'a');
    for (char& ch : word) {
      ch = static_cast<char>('a' + dist(rng));
    }
    if (seen.insert(word).second) {
      words.push_back(std::move(word));
    }
  }
  return words;
}

LC_PROBLEM(843, "Guess The Word", "design", ctx) {
  ctx.test("example_1_and_invalid_guess", [&] {
    std::vector<std::string> words = {"acckzz", "ccbazz", "eiowzz", "abcczz"};
    Master judge("acckzz", words);
    ctx.expect_eq(judge.guess("aaaaaa"), -1);

    Master master("acckzz", words);
    Solution sol;
    sol.findSecretWord(words, master);
    ctx.expect_true(master.passed());
  });

  ctx.test("single_word", [&] {
    std::vector<std::string> words = {"abcdef"};
    Master master("abcdef", words);
    Solution sol;
    sol.findSecretWord(words, master);
    ctx.expect_true(master.passed());
    ctx.expect_eq(master.guessesMade(), 1);
  });

  ctx.test("hundred_random_words_multiple_secrets", [&] {
    std::vector<std::string> words = makeRandomWordlist(100uz, 42U);
    Solution sol;

    for (const std::size_t secret_idx : {0uz, 37uz, 73uz, 99uz}) {
      Master master(words[secret_idx], words, 10);
      sol.findSecretWord(words, master);
      ctx.expect_true(master.passed());
    }
  });

  ctx.test("clustered_shared_suffix_wordlist", [&] {
    // All words share suffix "zzzz", so no pair has 0 matches; full minimax
    // still partitions candidates cleanly across match buckets 4..5.
    std::vector<std::string> words = {
        "aazzzz", "abzzzz", "aczzzz", "bazzzz", "bbzzzz",
        "bczzzz", "cazzzz", "cbzzzz", "cczzzz", "ddzzzz",
    };
    Master master("cbzzzz", words, 10);
    Solution sol;
    sol.findSecretWord(words, master);
    ctx.expect_true(master.passed());
  });
}

}  // namespace
