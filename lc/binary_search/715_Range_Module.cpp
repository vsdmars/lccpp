// LeetCode 715: Range Module
// https://leetcode.com/problems/range-module/
//
// Problem:
//   Design a RangeModule data structure that tracks sets of real numbers via
//   half-open intervals `[left, right)` and supports three operations:
//     - `addRange(left, right)`: start tracking every real number in `[left, right)`.
//     - `queryRange(left, right)`: return true iff every real number in
//       `[left, right)` is currently tracked.
//     - `removeRange(left, right)`: stop tracking every real number in
//       `[left, right)`.
//   Constraints: 1 <= left < right <= 10^9; up to 10^4 total calls.
//
// Examples:
//   Example 1:
//     Input:  RangeModule(); addRange(10, 20); removeRange(14, 16); queryRange(10,
//             14); queryRange(13, 15); queryRange(16, 17)
//     Output: [null, null, null, true, false, true]
//     Explanation:
//       - Initialize segments_ with sentinels [(-inf, false), (+inf, false)].
//       - addRange(10, 20) inserts boundaries 10 and 20 and sets segment (10,
//         true), yielding [(-inf, false), (10, true), (20, false), (+inf, false)].
//       - removeRange(14, 16) splits [10, 20) at 14 and 16 and marks [14, 16)
//         false, leaving [(-inf, false), (10, true), (14, false), (16, true), (20,
//         false), (+inf, false)].
//       - queryRange(10, 14) checks segments in [idx(10), idx(14)) and sees only
//         (10, true), returning true.
//       - queryRange(13, 15) spans [(10, true), (14, false)], sees (14, false), and
//         returns false; queryRange(16, 17) checks (16, true) and returns true.
//
//   Example 2:
//     Input:  RangeModule(); addRange(150, 200); addRange(250, 500); addRange(50,
//             100); addRange(180, 300); queryRange(150, 500); queryRange(100, 150);
//             addRange(100, 150); queryRange(50, 500)
//     Output: [null, null, null, null, null, true, false, null, true]
//     Explanation:
//       - After the first three addRange calls, segments_ alternates between false
//         and true at boundaries 50, 100, 150, 200, 250, and 500.
//       - addRange(180, 300) inserts 180 and 300, collapses intermediate boundaries
//         200 and 250, and coalesces with adjacent true segments at 150 and 300 to
//         form (150, true) followed by (500, false).
//       - queryRange(150, 500) verifies (150, true) and returns true, whereas
//         queryRange(100, 150) inspects (100, false) and returns false.
//       - addRange(100, 150) flips (100, false) to true and coalesces with both
//         (50, true) and (150, true), leaving a single tracked segment [50, 500) so
//         queryRange(50, 500) returns true.
//
//   Example 3:
//     Input:  RangeModule(); addRange(1, 100); removeRange(1, 100); queryRange(1,
//             2); addRange(20, 40); queryRange(20, 40); queryRange(19, 40)
//     Output: [null, null, null, false, null, true, false]
//     Explanation:
//       - addRange(1, 100) transitions segments_ to [(-inf, false), (1, true),
//         (100, false), (+inf, false)].
//       - removeRange(1, 100) sets (1, false) and coalesces with both (100, false)
//         and (-inf, false), collapsing segments_ back to the two sentinels [(-inf,
//         false), (+inf, false)].
//       - queryRange(1, 2) maps via upper_bound(1) - 1 to (-inf, false) and returns
//         false.
//       - addRange(20, 40) inserts (20, true) and (40, false), so queryRange(20,
//         40) returns true while queryRange(19, 40) includes (-inf, false) and
//         returns false.
//
// Algorithm:
//   Partition the entire real line `[-inf, +inf)` into a sorted sequence of
//   maximal half-open segments `[segments_[i].start, segments_[i+1].start)`,
//   each annotated with a boolean `tracked` flag (initialized with sentinels
//   `(-inf, false)` and `(+inf, false)`).
//   - `ensureBoundary(pivot)`: binary-search with `std::ranges::lower_bound`;
//     if `pivot` is not yet an explicit boundary, split the containing segment
//     by inserting `{pivot, segments_[idx - 1].tracked}`.
//   - `updateRange(left, right, tracked)` (used by both `addRange` and
//     `removeRange`): ensure boundaries at `left` and `right`, collapse every
//     intermediate boundary in `(lidx, ridx)` into a single segment starting at
//     `left` with state `tracked`, and coalesce adjacent segments that now share
//     the same `tracked` state. Coalescing preserves the alternating invariant
//     (`false, true, false, ...`) and keeps the segment vector minimal.
//   - `queryRange(left, right)`: locate the segment covering `left` via
//     `upper_bound(left) - 1` and the first boundary `>= right` via
//     `lower_bound(right)`, then verify all segments in `[lidx, ridx)` are
//     tracked.
//   Time Complexity: O(n) worst-case per `addRange`/`removeRange` (due to
//   vector erase/insert on `n` disjoint segments; O(log n) binary search),
//   O(log n) per `queryRange` (with coalesced alternating segments, `[lidx, ridx)`
//   returns false on at most the second segment).
//   Space Complexity: O(n), where `n` is the number of disjoint tracked ranges.
//
// Data Structures:
//   - Sorted `std::vector<Segment>` storing `{start, tracked}` boundary entries
//     with 64-bit `-inf` / `+inf` sentinels (preserving the Python coordinate-
//     partitioning approach cleanly without parallel-array desync or
//     floating-point precision issues).
//
// Real-World Applications:
//   - Operating-system virtual memory area (VMA) and page-range tracking
//     (`mmap` / `munmap` / `mprotect`).
//   - Disk extent allocators and free-space interval maps in filesystems.
//   - Calendar availability engines, IP CIDR block merging, and video/audio
//     timeline scrubbing buffers.
//
// Related LeetCode Problems:
//   - #56 Merge Intervals [done] (offline merging of overlapping intervals)
//   - #57 Insert Interval [done] (inserting a single interval into disjoint sorted intervals)
//   - #352 Data Stream as Disjoint Intervals [done] (maintaining merged intervals online)
//   - #729 My Calendar I [done] (non-overlapping interval booking)
//   - #732 My Calendar III [new] (boundary sweep-line over dynamic intervals)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace {

class RangeModule {
 public:
  // Initializes the real line `[-inf, +inf)` as completely untracked.
  RangeModule()
      : segments_{{.start = kNegInf, .tracked = false},
                  {.start = kPosInf, .tracked = false}} {}

  // Marks every real number in `[left, right)` as tracked.
  // Time: O(n) worst-case (O(log n) search + vector splice), Space: O(1) extra.
  void addRange(int left, int right) {
    if (left >= right) {
      return;
    }
    updateRange(left, right, true);
  }

  // Returns true iff every real number in `[left, right)` is currently tracked.
  // Time: O(log n) (at most 2 segments inspected because states alternate),
  // Space: O(1).
  [[nodiscard]] bool queryRange(int left, int right) const {
    if (left >= right) {
      return true;
    }
    const auto left64 = static_cast<std::int64_t>(left);
    const auto right64 = static_cast<std::int64_t>(right);

    // `upper_bound(left) - 1` finds the segment `[start, next_start)` whose
    // `start <= left < next_start`. Sentinels guarantee `it != begin()`.
    const auto left_it =
        std::ranges::upper_bound(segments_, left64, {}, &Segment::start) - 1;
    // `lower_bound(right)` is the first boundary `>= right`, which ends the
    // half-open range of segments intersecting `[left, right)`.
    const auto right_it =
        std::ranges::lower_bound(segments_, right64, {}, &Segment::start);

    return std::ranges::all_of(left_it, right_it,
                               [](const Segment& seg) { return seg.tracked; });
  }

  // Marks every real number in `[left, right)` as untracked.
  // Time: O(n) worst-case (O(log n) search + vector splice), Space: O(1) extra.
  void removeRange(int left, int right) {
    if (left >= right) {
      return;
    }
    updateRange(left, right, false);
  }

 private:
  struct Segment {
    std::int64_t start = 0;
    bool tracked = false;
  };

  static constexpr std::int64_t kNegInf = std::numeric_limits<std::int64_t>::min();
  static constexpr std::int64_t kPosInf = std::numeric_limits<std::int64_t>::max();

  // Ensures `pivot` exists as an explicit boundary in `segments_` and returns
  // its index. If `pivot` falls strictly inside an existing segment, that
  // segment is split without changing its `tracked` value.
  std::size_t ensureBoundary(std::int64_t pivot) {
    const auto it =
        std::ranges::lower_bound(segments_, pivot, {}, &Segment::start);
    const auto idx = static_cast<std::size_t>(it - segments_.begin());
    if (segments_[idx].start != pivot) {
      const bool inherited_state = segments_[idx - 1uz].tracked;
      segments_.insert(it, Segment{.start = pivot, .tracked = inherited_state});
    }
    return idx;
  }

  // Sets the tracked state for `[left, right)` to `tracked`, removing interior
  // boundaries and coalescing adjacent segments with identical states.
  void updateRange(int left, int right, bool tracked) {
    const std::size_t lidx = ensureBoundary(static_cast<std::int64_t>(left));
    const std::size_t ridx = ensureBoundary(static_cast<std::int64_t>(right));

    // Collapse all segments in `[lidx, ridx)` into the single segment at `lidx`.
    if (lidx + 1uz < ridx) {
      segments_.erase(
          segments_.begin() + static_cast<std::ptrdiff_t>(lidx + 1uz),
          segments_.begin() + static_cast<std::ptrdiff_t>(ridx));
    }
    segments_[lidx].tracked = tracked;

    // Coalesce with the right neighbor (never erasing the `+inf` sentinel).
    if (lidx + 1uz < segments_.size() - 1uz &&
        segments_[lidx + 1uz].tracked == segments_[lidx].tracked) {
      segments_.erase(
          segments_.begin() + static_cast<std::ptrdiff_t>(lidx + 1uz));
    }

    // Coalesce with the left neighbor (never erasing the `-inf` sentinel).
    if (lidx > 0uz && segments_[lidx - 1uz].tracked == segments_[lidx].tracked) {
      segments_.erase(segments_.begin() + static_cast<std::ptrdiff_t>(lidx));
    }
  }

  std::vector<Segment> segments_;
};

LC_PROBLEM(715, "Range Module", "binary_search", ctx) {
  ctx.test("example_1", [&] {
    RangeModule rm;
    rm.addRange(10, 20);
    rm.removeRange(14, 16);
    ctx.expect_true(rm.queryRange(10, 14));
    ctx.expect_false(rm.queryRange(13, 15));
    ctx.expect_true(rm.queryRange(16, 17));
  });

  ctx.test("python_main_repeated_removals", [&] {
    RangeModule rm;
    rm.addRange(10, 20);
    rm.removeRange(14, 17);
    rm.removeRange(15, 16);
    rm.removeRange(14, 17);
    ctx.expect_true(rm.queryRange(10, 14));
    ctx.expect_false(rm.queryRange(14, 17));
    ctx.expect_false(rm.queryRange(13, 18));
    ctx.expect_true(rm.queryRange(17, 20));
  });

  ctx.test("merging_adjacent_and_overlapping_ranges", [&] {
    RangeModule rm;
    rm.addRange(150, 200);
    rm.addRange(250, 500);
    rm.addRange(50, 100);
    rm.addRange(180, 300);
    ctx.expect_true(rm.queryRange(150, 500));
    ctx.expect_false(rm.queryRange(100, 150));
    ctx.expect_false(rm.queryRange(600, 1000));

    // Bridge the remaining gap [100, 150) exactly at shared endpoints.
    rm.addRange(100, 150);
    ctx.expect_true(rm.queryRange(50, 500));
  });

  ctx.test("full_removal_and_readd", [&] {
    RangeModule rm;
    rm.addRange(1, 100);
    rm.removeRange(1, 100);
    ctx.expect_false(rm.queryRange(1, 2));
    ctx.expect_false(rm.queryRange(50, 60));

    rm.addRange(20, 40);
    ctx.expect_true(rm.queryRange(20, 40));
    ctx.expect_false(rm.queryRange(19, 40));
    ctx.expect_false(rm.queryRange(20, 41));
  });

  ctx.test("empty_module_and_large_coordinates", [&] {
    RangeModule rm;
    ctx.expect_false(rm.queryRange(1, 10));
    rm.addRange(100000000, 999999999);
    ctx.expect_true(rm.queryRange(500000000, 999999999));
    rm.removeRange(500000000, 500000001);
    ctx.expect_false(rm.queryRange(499999999, 500000002));
    ctx.expect_true(rm.queryRange(500000001, 999999999));
  });
}

}  // namespace
