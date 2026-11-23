// LeetCode 1024: Video Stitching
// https://leetcode.com/problems/video-stitching/
//
// Problem:
//   You are given a list of video clips where `clips[i] = [start_i, end_i]`
//   represents a clip covering the closed time interval `[start_i, end_i]`,
//   and a target duration `time`. Clips may overlap and can be trimmed freely.
//   Return the minimum number of clips required to cover the entire interval
//   `[0, time]`, or `-1` if complete coverage is impossible.
//
// Algorithm:
//   Greedy interval covering (Jump Game II frontier expansion):
//     1. Handle the trivial edge case `time <= 0` (0 clips required).
//     2. Sort `clips` in ascending order of start time.
//     3. Maintain `current_end` (initially 0), the farthest time covered by
//        the `num_clips` chosen so far.
//     4. At each greedy round, scan all clips whose `start <= current_end` and
//        track `farthest_end = max(clip[1])`.
//        - If the next unprocessed clip starts strictly after `current_end`,
//          or `farthest_end == current_end`, there is an unbridgeable gap:
//          return `-1`.
//        - Otherwise pick the clip achieving `farthest_end`, increment
//          `num_clips`, set `current_end = farthest_end`, and return
//          `num_clips` as soon as `current_end >= time`.
//   Each clip is inspected at most once after sorting.
//   Time Complexity: O(n log n) due to sorting, where n = clips.size().
//   Space Complexity: O(n) for the flat interval array.
//
// Data Structures:
//   - Flat sorted `std::vector<Clip>` of `{start, end}` intervals.
//   - Scalar frontier trackers (`current_end`, `farthest_end`, `idx`).
//
// Real-World Applications:
//   - Media editing and HLS/DASH stream assembly: stitching the fewest source
//     recordings to produce continuous event coverage.
//   - Wireless sensor and cellular coverage planning: selecting the minimum
//     number of overlapping coverage ranges along a corridor or highway.
//   - Shift and on-call roster scheduling: covering a 24-hour operational
//     window with the fewest available overlapping shifts.
//
// Related LeetCode Problems:
//   - #45 Jump Game II [done] (minimum jumps to reach the end — identical greedy frontier expansion)
//   - #55 Jump Game [done] (reachability version of greedy farthest-reach)
//   - #1326 Minimum Number of Taps to Open to Water a Garden [new] (same interval-covering problem converted from tap radii)
//   - #56 Merge Intervals [done] (sorting and sweeping overlapping intervals)
//   - #435 Non-overlapping Intervals [done] (greedy interval selection)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

// Compact value representation of a closed video clip interval `[start, end]`.
struct Clip {
  int start;
  int end;
};

class Solution {
 public:
  // Returns the minimum number of clips needed to cover [0, time], or -1 if
  // impossible, using sorted greedy frontier expansion.
  // Time: O(n log n), Space: O(n) for the flat interval copy.
  int videoStitching(const std::vector<std::vector<int>>& clips, int time) {
    if (time <= 0) {
      return 0;
    }

    // Copy into a flat trivially-copyable struct array before sorting for
    // better cache locality and zero inner-vector moves during sorting.
    std::vector<Clip> intervals;
    intervals.reserve(clips.size());
    for (const auto& clip : clips) {
      intervals.push_back(Clip{.start = clip[0], .end = clip[1]});
    }

    // Sort intervals by ascending start time so all clips eligible to extend
    // the current covered frontier `[0, current_end]` appear contiguously.
    std::ranges::sort(intervals, [](const Clip& lhs, const Clip& rhs) {
      return lhs.start < rhs.start;
    });

    int num_clips = 0;
    int current_end = 0;
    std::size_t idx = 0uz;

    while (idx < intervals.size()) {
      // Gap detected: no remaining clip starts on or before `current_end`.
      if (intervals[idx].start > current_end) {
        return -1;
      }

      // Among all clips that overlap/touch `current_end`, greedily choose the
      // one extending coverage the farthest to the right.
      int farthest_end = current_end;
      while (idx < intervals.size() && intervals[idx].start <= current_end) {
        farthest_end = std::max(farthest_end, intervals[idx].end);
        ++idx;
      }

      // No forward progress possible (e.g., all reachable clips end <= current_end).
      if (farthest_end == current_end) {
        return -1;
      }

      ++num_clips;
      current_end = farthest_end;
      if (current_end >= time) {
        return num_clips;
      }
    }

    return -1;
  }
};

LC_PROBLEM(1024, "Video Stitching", "sliding_window", ctx) {
  ctx.test("leetcode example 1 and python build", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.videoStitching({{0, 2}, {4, 6}, {8, 10}, {1, 9}, {1, 5}, {5, 9}},
                           10),
        3);
  });

  ctx.test("leetcode example 2 unreachable target", [&] {
    Solution sol;
    ctx.expect_eq(sol.videoStitching({{0, 1}, {1, 2}}, 5), -1);
  });

  ctx.test("leetcode example 3 dense overlapping clips", [&] {
    Solution sol;
    ctx.expect_eq(
        sol.videoStitching({{0, 1},
                            {6, 8},
                            {0, 2},
                            {5, 6},
                            {0, 4},
                            {0, 3},
                            {6, 7},
                            {1, 3},
                            {4, 7},
                            {1, 4},
                            {2, 5},
                            {2, 6},
                            {3, 4},
                            {4, 5},
                            {5, 7},
                            {6, 9}},
                           9),
        3);
  });

  ctx.test("gap at start time zero", [&] {
    Solution sol;
    ctx.expect_eq(sol.videoStitching({{1, 5}, {4, 10}}, 10), -1);
  });

  ctx.test("zero target duration needs zero clips", [&] {
    Solution sol;
    ctx.expect_eq(sol.videoStitching({{0, 2}}, 0), 0);
  });

  ctx.test("zero-length stalled clips", [&] {
    Solution sol;
    ctx.expect_eq(sol.videoStitching({{0, 0}, {0, 0}}, 1), -1);
  });
}

}  // namespace
