// LeetCode 1032: Stream of Characters
// https://leetcode.com/problems/stream-of-characters/
//
// Problem:
//   Design a `StreamChecker` class initialized with an array of lowercase
//   English strings `words` (1 <= words.length <= 2000,
//   1 <= words[i].length <= 200). Each call to `query(letter)` (up to
//   4 * 10^4 calls) appends `letter` to an incoming character stream and
//   returns `true` if any non-empty suffix of the stream matches a word in
//   `words`, or `false` otherwise.
//
// Algorithm:
//   Builds a reversed Trie during construction by inserting every word from
//   its last character to its first. Reversing the dictionary turns suffix
//   matching on the live stream into a standard root-to-node prefix traversal
//   over the newest characters. The constructor also records `max_word_len_`
//   so the stream buffer never keeps characters older than the longest word.
//   On each `query(letter)`, `letter` is pushed to the front of `stream_`
//   (evicting the oldest character if `stream_.size() > max_word_len_`), and
//   the trie is traversed from root 0 along `stream_`: returning `true` as
//   soon as an `is_end` node is encountered, or `false` on a missing child.
//   - Constructor: Time O(sum(|words[i]|)), Space O(sum(|words[i]|) * 26).
//   - `query`:     Time O(max_word_len_), Space O(max_word_len_) total buffer
//     (O(1) per call).
//
// Data Structures:
//   - `std::vector<Node>` (flat arena-allocated Trie): Contiguous node pool
//     where index 0 is the root and `kNoChild` (0) denotes an absent edge;
//     each `Node` stores `std::array<std::size_t, 26> children` and `is_end`.
//   - `std::deque<char>`: Bounded sliding window of the most recent
//     `max_word_len_` characters stored newest-first for O(1) push/pop and
//     forward traversal.
//
// Real-World Applications:
//   - Network intrusion detection systems (IDS) and deep packet inspection
//     matching live byte streams against signature dictionaries.
//   - Real-time profanity filters, DLP (Data Loss Prevention) scanners, and
//     streaming log keyword alerting.
//   - Streaming lexical scanners and bioinformatics motif matchers scanning
//     continuous symbol sequences.
//
// Related LeetCode Problems:
//   - #208 Implement Trie (Prefix Tree) (core prefix tree insert and search)
//   - #211 Design Add and Search Words Data Structure (trie traversal with
//     wildcard matching)
//   - #745 Prefix and Suffix Search (trie indexing for prefix/suffix queries)
//   - #139 Word Break (matching dictionary words against string segments)
//   - #3045 Count Prefix and Suffix Pairs II (trie-based prefix and suffix
//     matching across word pairs)

#include "framework/lc_framework.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <deque>
#include <string>
#include <vector>

namespace {

constexpr std::size_t kAlphabetSize = 26uz;
constexpr std::size_t kNoChild = 0uz;

constexpr std::size_t char_index(char c) {
  return static_cast<std::size_t>(static_cast<unsigned char>(c)) -
         static_cast<std::size_t>(static_cast<unsigned char>('a'));
}

// Reversed Trie + bounded recent-character deque.
// Insert every word in reverse so suffix checks become root-to-leaf prefix
// traversals over the newest characters. Bounding the deque to `max_word_len_`
// keeps query memory O(max word length).
// Constructor: Time O(sum of word lengths), Space O(sum of word lengths * 26).
// query:       Time O(max word length), Space O(1) amortized.
class StreamChecker {
 public:
  explicit StreamChecker(const std::vector<std::string>& words)
      : nodes_(1uz) {
    for (const std::string& word : words) {
      max_word_len_ = std::max(max_word_len_, word.size());
      std::size_t cur = 0uz;
      for (std::size_t i = word.size(); i > 0uz; --i) {
        const std::size_t idx = char_index(word[i - 1uz]);
        if (nodes_[cur].children[idx] == kNoChild) {
          const std::size_t next_id = nodes_.size();
          nodes_[cur].children[idx] = next_id;
          nodes_.emplace_back();
        }
        cur = nodes_[cur].children[idx];
      }
      nodes_[cur].is_end = true;
    }
  }

  bool query(char letter) {
    if (max_word_len_ == 0uz) {
      return false;
    }
    stream_.push_front(letter);
    if (stream_.size() > max_word_len_) {
      stream_.pop_back();
    }

    std::size_t cur = 0uz;
    for (const char c : stream_) {
      const std::size_t next = nodes_[cur].children[char_index(c)];
      if (next == kNoChild) {
        return false;
      }
      cur = next;
      if (nodes_[cur].is_end) {
        return true;
      }
    }
    return false;
  }

 private:
  struct Node {
    std::array<std::size_t, kAlphabetSize> children{};
    bool is_end = false;
  };

  std::vector<Node> nodes_;
  std::deque<char> stream_;
  std::size_t max_word_len_ = 0uz;
};

LC_PROBLEM(1032, "Stream Of Characters", "trie", ctx) {
  ctx.test("leetcode_example", [&] {
    StreamChecker checker({"cd", "f", "kl"});
    const std::string letters = "abcdefghijkl";
    const std::vector<bool> expected{false, false, false, true,  false, true,
                                     false, false, false, false, false, true};
    for (std::size_t i = 0uz; i < letters.size(); ++i) {
      ctx.expect_eq(checker.query(letters[i]), expected[i]);
    }
  });

  ctx.test("overlapping_suffixes_and_duplicates", [&] {
    StreamChecker checker({"ab", "bab", "ab", "xyz"});
    ctx.expect_false(checker.query('b'));
    ctx.expect_false(checker.query('a'));
    ctx.expect_true(checker.query('b'));  // matches "ab" and "bab"
    ctx.expect_false(checker.query('x'));
    ctx.expect_false(checker.query('y'));
    ctx.expect_true(checker.query('z'));  // matches "xyz"
  });

  ctx.test("single_letter_word", [&] {
    StreamChecker checker({"a"});
    ctx.expect_true(checker.query('a'));
    ctx.expect_false(checker.query('b'));
    ctx.expect_true(checker.query('a'));
  });
}

}  // namespace
