// https://leetcode.com/problems/accounts-merge/description/
//
// Given a list of `accounts` where `accounts[i][0]` is a person's name and
// `accounts[i][1..]` are emails belonging to that account, merge all accounts
// that share at least one email. Return merged accounts with each account's
// emails sorted in ascending order.

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

class Solution {
 public:
  // DFS over the bipartite email <-> account-index graph.
  // Time: O(N * K log(N * K)), Space: O(N * K) where N*K is total emails.
  std::vector<std::vector<std::string>> accountsMerge(
      const std::vector<std::vector<std::string>>& accounts) {
    // Map each email to every account index that lists it.
    std::unordered_map<std::string, std::vector<std::size_t>> email_to_accts;
    for (std::size_t idx = 0uz; idx < accounts.size(); ++idx) {
      for (std::size_t j = 1uz; j < accounts[idx].size(); ++j) {
        email_to_accts[accounts[idx][j]].push_back(idx);
      }
    }

    std::vector<std::uint8_t> visited(accounts.size(), 0);
    std::vector<std::vector<std::string>> merged;

    auto dfs = [&](this auto&& self, std::size_t acct_idx,
                   std::unordered_set<std::string>& collected) -> void {
      visited[acct_idx] = 1;
      for (std::size_t j = 1uz; j < accounts[acct_idx].size(); ++j) {
        const std::string& email = accounts[acct_idx][j];
        if (!collected.insert(email).second) {
          continue;
        }
        for (const std::size_t next_acct : email_to_accts[email]) {
          if (visited[next_acct] == 0) {
            self(next_acct, collected);
          }
        }
      }
    };

    for (std::size_t i = 0uz; i < accounts.size(); ++i) {
      if (visited[i] != 0) {
        continue;
      }
      std::unordered_set<std::string> emails;
      dfs(i, emails);

      std::vector<std::string> entry;
      entry.reserve(emails.size() + 1uz);
      entry.push_back(accounts[i][0]);
      for (const auto& email : emails) {
        entry.push_back(email);
      }
      std::sort(entry.begin() + 1, entry.end());
      merged.push_back(std::move(entry));
    }

    return merged;
  }

  // Iterative graph traversal connecting emails in the same account as a star
  // around the first email (mirrors bfs_solution() in 721_Accounts_Merge.py).
  std::vector<std::vector<std::string>> bfsSolution(
      const std::vector<std::vector<std::string>>& accounts) {
    std::unordered_map<std::string, std::string> email_to_name;
    std::unordered_map<std::string, std::vector<std::string>> graph;

    for (const auto& acct : accounts) {
      if (acct.size() < 2uz) {
        continue;
      }
      const std::string& name = acct[0];
      const std::string& anchor = acct[1];
      for (std::size_t j = 1uz; j < acct.size(); ++j) {
        const std::string& email = acct[j];
        graph[anchor].push_back(email);
        graph[email].push_back(anchor);
        email_to_name[email] = name;
      }
    }

    std::unordered_set<std::string> visited;
    std::vector<std::vector<std::string>> merged;

    for (const auto& [start_email, _] : graph) {
      if (!visited.insert(start_email).second) {
        continue;
      }

      std::vector<std::string> stack = {start_email};
      std::vector<std::string> component;
      while (!stack.empty()) {
        std::string cur = std::move(stack.back());
        stack.pop_back();
        component.push_back(cur);

        for (const auto& nbr : graph[cur]) {
          if (visited.insert(nbr).second) {
            stack.push_back(nbr);
          }
        }
      }

      std::ranges::sort(component);
      std::vector<std::string> entry;
      entry.reserve(component.size() + 1uz);
      entry.push_back(email_to_name[start_email]);
      for (auto& email : component) {
        entry.push_back(std::move(email));
      }
      merged.push_back(std::move(entry));
    }

    return merged;
  }
};

// Canonicalizes outer account ordering so tests are order-independent.
std::vector<std::vector<std::string>> canonicalize(
    std::vector<std::vector<std::string>> accounts) {
  std::ranges::sort(accounts);
  return accounts;
}

LC_PROBLEM(721, "Accounts Merge", "graph", ctx) {
  Solution sol;

  ctx.test("example 1: merge shared John emails, keep distinct John & Mary",
           [&] {
             const std::vector<std::vector<std::string>> accounts = {
                 {"John", "johnsmith@mail.com", "john_newyork@mail.com"},
                 {"John", "johnsmith@mail.com", "john00@mail.com"},
                 {"Mary", "mary@mail.com"},
                 {"John", "johnnybravo@mail.com"},
             };
             const std::vector<std::vector<std::string>> expected =
                 canonicalize({
                     {"John", "john00@mail.com", "john_newyork@mail.com",
                      "johnsmith@mail.com"},
                     {"John", "johnnybravo@mail.com"},
                     {"Mary", "mary@mail.com"},
                 });
             ctx.expect_eq(canonicalize(sol.accountsMerge(accounts)), expected);
             ctx.expect_eq(canonicalize(sol.bfsSolution(accounts)), expected);
           });

  ctx.test("transitive chain merge across three accounts", [&] {
    const std::vector<std::vector<std::string>> accounts = {
        {"Alex", "a@m.com", "b@m.com"},
        {"Alex", "c@m.com", "d@m.com"},
        {"Alex", "b@m.com", "c@m.com"},
    };
    const std::vector<std::vector<std::string>> expected = {
        {"Alex", "a@m.com", "b@m.com", "c@m.com", "d@m.com"},
    };
    ctx.expect_eq(canonicalize(sol.accountsMerge(accounts)), expected);
    ctx.expect_eq(canonicalize(sol.bfsSolution(accounts)), expected);
  });
}

}  // namespace
