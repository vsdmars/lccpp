// LeetCode 1242: Web Crawler Multithreaded
// https://leetcode.com/problems/web-crawler-multithreaded/
//
// Problem:
//   Given a starting URL `startUrl` and an `HtmlParser` interface whose
//   blocking `getUrls(url)` method returns all hyperlinks found on a webpage,
//   implement a multithreaded web crawler that discovers and returns all URLs
//   reachable from `startUrl` under the exact same hostname (in any order).
//   Requirements:
//   - Start crawling from `startUrl`.
//   - Only crawl URLs sharing the same hostname as `startUrl` (assume `http://`
//     protocol with no port number).
//   - Never crawl any URL more than once.
//   - Execute blocking `getUrls` calls concurrently across multiple threads.
//
// Algorithm:
//   Fixed-size Worker-Pool Concurrent BFS:
//   1. Extract `target_host` from `startUrl` by slicing between `"://"` and the
//      first `'/'` after the host.
//   2. Initialize shared synchronization state protected by a `std::mutex` and
//      `std::condition_variable`:
//      - `pending`: queue seeded with `startUrl`.
//      - `visited`: hash set seeded with `startUrl` (deduplicates before
//        enqueuing so no URL is ever fetched twice).
//      - `active_workers`: number of threads currently executing a blocking
//        `getUrls()` call outside the lock.
//      - `done`: termination flag set only when `pending.empty()` AND
//        `active_workers == 0`.
//   3. Spawn `kWorkerCount` threads. Each worker loops:
//      a) Lock mutex and wait until `!pending.empty() || done`.
//      b) If `done`, exit cleanly; otherwise pop one URL and increment
//         `active_workers`.
//      c) Release the lock and call `html_parser.getUrls(url)` concurrently.
//      d) Re-acquire the lock, filter returned URLs by `target_host`, insert
//         unseen ones into `visited` and `pending`, decrement `active_workers`,
//         and either set `done = true` (if no work remains anywhere) or wake
//         waiting workers with `notify_all()`.
//   4. Join all workers and return the elements of `visited`.
//   Time Complexity:  O((V + E) * L) total CPU work across threads (wall-clock
//                     I/O latency reduced up to `kWorkerCount`x), where V is
//                     reachable same-host URLs, E is inspected links, and L is
//                     average URL length.
//   Space Complexity: O(V * L) — visited set and pending frontier queue.
//
// Data Structures:
//   - Shared frontier queue (`std::queue<std::string>`): distributes URLs to
//     idle worker threads in FIFO order.
//   - Visited hash set (`std::unordered_set<std::string>`): enforces the
//     exactly-once crawl invariant under mutex protection.
//   - Synchronization primitives (`std::mutex`, `std::condition_variable`):
//     coordinate producer-consumer handoff and deadlock-free termination with
//     zero busy waiting.
//   - Worker pool (`std::vector<std::thread>`): executes blocking HTTP fetches
//     in parallel.
//
// Real-World Applications:
//   - Search-engine & site-map web crawlers: concurrently fetching domain-
//     scoped pages while respecting per-host deduplication frontiers.
//   - Link checkers & security scanners: parallel traversal of internal web
//     applications to detect broken links or misconfigured endpoints.
//   - Distributed asset/dependency downloaders: parallel BFS traversal of
//     package or container-layer manifests using bounded worker pools.
//
// Related LeetCode Problems:
//   - #1236 Web Crawler [new] (single-threaded version of same-hostname BFS crawling)
//   - #133 Clone Graph [done] (graph BFS/DFS with visited-set deduplication on cyclic directed graphs)
//   - #1188 Design Bounded Blocking Queue [done] (thread-safe producer-consumer queue using mutex and condition variables)
//   - #1195 Fizz Buzz Multithreaded [new] (coordinating multiple worker threads with condition variables)

#include "framework/lc_framework.h"

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

constexpr std::string_view kSchemeSeparator = "://";
constexpr std::size_t kWorkerCount = 4uz;

// Mock of LeetCode's blocking HtmlParser interface. Thread-safe for concurrent
// `getUrls` calls and records per-URL fetch counts so tests can verify that no
// URL is ever crawled twice.
class HtmlParser {
 public:
  explicit HtmlParser(
      std::unordered_map<std::string, std::vector<std::string>> graph,
      int delay_ms = 0)
      : graph_(std::move(graph)), delay_ms_(delay_ms) {}

  // Simulates a blocking HTTP fetch returning all outgoing links on `url`.
  [[nodiscard]] std::vector<std::string> getUrls(
      const std::string& url) const {
    {
      std::scoped_lock<std::mutex> lock(call_mu_);
      ++call_counts_[url];
    }
    if (delay_ms_ > 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms_));
    }
    if (const auto it = graph_.find(url); it != graph_.end()) {
      return it->second;
    }
    return {};
  }

  // Returns how many times `getUrls` was invoked for `url`.
  [[nodiscard]] int callCount(const std::string& url) const {
    std::scoped_lock<std::mutex> lock(call_mu_);
    if (const auto it = call_counts_.find(url); it != call_counts_.end()) {
      return it->second;
    }
    return 0;
  }

 private:
  std::unordered_map<std::string, std::vector<std::string>> graph_;
  int delay_ms_ = 0;
  mutable std::mutex call_mu_;
  mutable std::unordered_map<std::string, int> call_counts_;
};

// Shared synchronization state for the concurrent BFS frontier.
struct CrawlState {
  std::string target_host;
  std::mutex mu;
  std::condition_variable cv;
  std::queue<std::string> pending;
  std::unordered_set<std::string> visited;
  int active_workers = 0;
  bool done = false;
};

class Solution {
 public:
  // Concurrently crawls all URLs reachable from `start_url` under the same
  // hostname using a fixed worker pool and returns all discovered URLs.
  // Time: O((V + E) * L), Space: O(V * L).
  std::vector<std::string> crawl(const std::string& start_url,
                                 const HtmlParser& html_parser) {
    CrawlState state;
    state.target_host = std::string(extractHostname(start_url));
    state.visited.insert(start_url);
    state.pending.push(start_url);

    // Launch worker threads that share `state` until all reachable same-host
    // URLs have been fetched and no worker remains active.
    std::vector<std::thread> workers;
    workers.reserve(kWorkerCount);
    for (std::size_t i = 0uz; i < kWorkerCount; ++i) {
      workers.emplace_back(workerLoop, std::ref(state), std::cref(html_parser));
    }

    for (auto& worker : workers) {
      worker.join();
    }

    return {state.visited.begin(), state.visited.end()};
  }

 private:
  // Extracts the hostname substring from an `http://<host>[/<path>]` URL.
  static std::string_view extractHostname(std::string_view url) {
    const std::size_t scheme_pos = url.find(kSchemeSeparator);
    const std::size_t host_start =
        scheme_pos == std::string_view::npos
            ? 0uz
            : scheme_pos + kSchemeSeparator.size();
    const std::size_t slash_pos = url.find('/', host_start);
    if (slash_pos == std::string_view::npos) {
      return url.substr(host_start);
    }
    return url.substr(host_start, slash_pos - host_start);
  }

  // Worker routine: repeatedly claims a queued URL, fetches its links outside
  // the lock, and enqueues any unseen same-host neighbors.
  static void workerLoop(CrawlState& state, const HtmlParser& html_parser) {
    while (true) {
      std::string current_url;
      {
        std::unique_lock<std::mutex> lock(state.mu);
        state.cv.wait(
            lock, [&state]() { return !state.pending.empty() || state.done; });
        if (state.done) {
          return;
        }
        current_url = std::move(state.pending.front());
        state.pending.pop();
        ++state.active_workers;
      }

      // Blocking I/O runs unlocked so all workers execute requests in parallel.
      std::vector<std::string> neighbors = html_parser.getUrls(current_url);

      {
        std::scoped_lock<std::mutex> lock(state.mu);
        for (std::string& next_url : neighbors) {
          if (extractHostname(next_url) == state.target_host &&
              state.visited.insert(next_url).second) {
            state.pending.push(std::move(next_url));
          }
        }
        --state.active_workers;

        // Termination invariant: crawling is complete if and only if no URLs
        // remain queued and no other worker is currently mid-fetch.
        if (state.pending.empty() && state.active_workers == 0) {
          state.done = true;
          state.cv.notify_all();
        } else if (!state.pending.empty()) {
          state.cv.notify_all();
        }
      }
    }
  }
};

// Sorts crawled URLs for deterministic equality assertions.
std::vector<std::string> sortedUrls(std::vector<std::string> urls) {
  std::ranges::sort(urls);
  return urls;
}

LC_PROBLEM(1242, "Web Crawler Multithreaded", "bfs", ctx) {
  Solution sol;

  ctx.test("example1_same_hostname_filtering", [&] {
    const std::unordered_map<std::string, std::vector<std::string>> graph = {
        {"http://news.yahoo.com",
         {"http://news.yahoo.com/news", "http://news.google.com"}},
        {"http://news.yahoo.com/news",
         {"http://news.yahoo.com/news/topics/", "http://news.yahoo.com/us"}},
        {"http://news.yahoo.com/news/topics/",
         {"http://news.yahoo.com", "http://news.yahoo.com/news"}},
        {"http://news.google.com",
         {"http://news.yahoo.com/news/topics/", "http://news.yahoo.com/us"}},
    };
    const HtmlParser parser(graph);

    const std::vector<std::string> actual =
        sortedUrls(sol.crawl("http://news.yahoo.com/news/topics/", parser));
    const std::vector<std::string> expected = {
        "http://news.yahoo.com",
        "http://news.yahoo.com/news",
        "http://news.yahoo.com/news/topics/",
        "http://news.yahoo.com/us",
    };

    ctx.expect_eq(actual, expected);
    for (const auto& url : expected) {
      ctx.expect_eq(parser.callCount(url), 1);
    }
    ctx.expect_eq(parser.callCount("http://news.google.com"), 0);
  });

  ctx.test("example2_only_external_links_from_start", [&] {
    const std::unordered_map<std::string, std::vector<std::string>> graph = {
        {"http://news.google.com",
         {"http://news.yahoo.com", "http://news.yahoo.com/news"}},
        {"http://news.yahoo.com", {"http://news.google.com"}},
    };
    const HtmlParser parser(graph);

    const std::vector<std::string> actual =
        sortedUrls(sol.crawl("http://news.google.com", parser));
    ctx.expect_eq(actual, std::vector<std::string>{"http://news.google.com"});
    ctx.expect_eq(parser.callCount("http://news.google.com"), 1);
    ctx.expect_eq(parser.callCount("http://news.yahoo.com"), 0);
  });

  ctx.test("cyclic_diamond_graph_with_simulated_latency", [&] {
    // Exercises concurrent fan-out, overlapping paths, cycles, and external
    // host exclusion under simulated blocking I/O delay.
    const std::unordered_map<std::string, std::vector<std::string>> graph = {
        {"http://example.org/a",
         {"http://example.org/b", "http://example.org/c",
          "http://example.com/external"}},
        {"http://example.org/b",
         {"http://example.org/d", "http://example.org/a"}},
        {"http://example.org/c",
         {"http://example.org/d", "http://example.org/e"}},
        {"http://example.org/d", {"http://example.org/a"}},
        {"http://example.org/e", {"http://example.org/b"}},
        // Unreachable same-host island must not be crawled.
        {"http://example.org/isolated", {"http://example.org/a"}},
    };
    const HtmlParser parser(graph, /*delay_ms=*/2);

    const std::vector<std::string> actual =
        sortedUrls(sol.crawl("http://example.org/a", parser));
    const std::vector<std::string> expected = {
        "http://example.org/a", "http://example.org/b", "http://example.org/c",
        "http://example.org/d", "http://example.org/e",
    };

    ctx.expect_eq(actual, expected);
    for (const auto& url : expected) {
      ctx.expect_eq(parser.callCount(url), 1);
    }
    ctx.expect_eq(parser.callCount("http://example.com/external"), 0);
    ctx.expect_eq(parser.callCount("http://example.org/isolated"), 0);
  });

  ctx.test("single_isolated_url", [&] {
    const HtmlParser parser({});
    const std::vector<std::string> actual =
        sol.crawl("http://leetcode.com/problems", parser);
    ctx.expect_eq(actual,
                  std::vector<std::string>{"http://leetcode.com/problems"});
    ctx.expect_eq(parser.callCount("http://leetcode.com/problems"), 1);
  });
}

}  // namespace
