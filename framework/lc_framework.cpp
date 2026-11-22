#include "framework/lc_framework.h"

#include <algorithm>
#include <filesystem>
#include <iostream>

namespace lc {

void TestContext::fail(std::source_location loc, std::string detail) {
  const std::string file =
      std::filesystem::path(loc.file_name()).filename().string();
  const std::string msg =
      file + ":" + std::to_string(loc.line()) + ": " + std::move(detail);
  if (current_ != nullptr) {
    current_->passed = false;
    current_->failures.push_back(msg);
  } else {
    std::cerr << "[FAIL] " << msg << '\n';
  }
}

bool TestContext::all_passed() const noexcept {
  return std::ranges::all_of(results_, &CaseResult::passed);
}

std::size_t TestContext::passed_count() const noexcept {
  return static_cast<std::size_t>(
      std::ranges::count_if(results_, &CaseResult::passed));
}

Registry& Registry::instance() {
  static Registry reg;
  return reg;
}

void Registry::add(ProblemEntry entry) {
  entries_.push_back(std::move(entry));
}

std::vector<const ProblemEntry*> Registry::sorted_all() const {
  std::vector<const ProblemEntry*> out;
  out.reserve(entries_.size());
  for (const auto& entry : entries_) {
    out.push_back(&entry);
  }
  std::ranges::sort(out, {}, &ProblemEntry::id);
  return out;
}

const ProblemEntry* Registry::find_by_id(int id) const {
  const auto it = std::ranges::find(entries_, id, &ProblemEntry::id);
  return it != entries_.end() ? &(*it) : nullptr;
}

}  // namespace lc
