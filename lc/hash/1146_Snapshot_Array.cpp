// LeetCode 1146: Snapshot Array
// https://leetcode.com/problems/snapshot-array/
//
// Problem:
//   Implement a `SnapshotArray` data structure supporting:
//   - `SnapshotArray(int length)`: initializes an array of `length` elements,
//     all initially `0`.
//   - `void set(int index, int val)`: sets the element at `index` to `val`.
//   - `int snap()`: takes a snapshot and returns its `snap_id` (0-indexed count
//     of previous `snap()` calls).
//   - `int get(int index, int snap_id)`: returns the value at `index` at the
//     time snapshot `snap_id` was taken.
//   Constraints: 1 <= length <= 50000, at most 50000 total calls to `set`,
//   `snap`, and `get`, 0 <= index < length, 0 <= val <= 10^9.
//
// Examples:
//   Example 1:
//     Input:  ["SnapshotArray", "set", "snap", "set", "get", "snap", "get", "get"],
//             [[3], [0, 5], [], [0, 6], [0, 0], [], [0, 1], [1, 1]]
//     Output: [null, null, 0, null, 5, 1, 6, 0]
//     Explanation:
//       - SnapshotArray(3) allocates 3 empty version vectors in history_ and
//         initializes snap_id_ = 0.
//       - set(0, 5) appends {0, 5} to history_[0], snap() returns 0 and increments
//         snap_id_ to 1, and set(0, 6) appends {1, 6} to history_[0].
//       - get(0, 0) runs upper_bound for 0 on [{0, 5}, {1, 6}], landing on {1, 6}
//         and returning std::prev(it)->value = 5.
//       - snap() returns 1 (snap_id_ becomes 2), get(0, 1) lands at end() and
//         returns std::prev(it)->value = 6, and get(1, 1) hits history_[1].begin()
//         and returns the default 0.
//
//   Example 2:
//     Input:  ["SnapshotArray", "set", "set", "set", "snap", "set", "set", "snap",
//             "get", "get"], [[4], [2, 10], [2, 25], [2, 0], [], [2, 99], [2, 42],
//             [], [2, 0], [2, 1]]
//     Output: [null, null, null, null, 0, null, null, 1, 0, 42]
//     Explanation:
//       - SnapshotArray(4) initializes 4 empty logs with snap_id_ = 0; set(2, 10)
//         appends {0, 10} to history_[2], and set(2, 25) then set(2, 0) overwrite
//         versions.back() in place because versions.back().snap_id == 0.
//       - snap() returns 0 and advances snap_id_ to 1; set(2, 99) appends {1, 99}
//         and set(2, 42) overwrites it in place to leave history_[2] = [{0, 0}, {1,
//         42}], then snap() returns 1.
//       - get(2, 0) uses upper_bound(0) to locate {1, 42} and return
//         std::prev(it)->value = 0, while get(2, 1) reaches end() and returns
//         std::prev(it)->value = 42.
//
//   Example 3:
//     Input:  ["SnapshotArray", "snap", "set", "set", "snap", "snap", "snap",
//             "get", "get", "get", "get", "get", "get"], [[2], [], [1, 17], [0,
//             20], [], [], [], [0, 0], [1, 0], [0, 1], [1, 1], [0, 3], [1, 3]]
//     Output: [null, 0, null, null, 1, 2, 3, 0, 0, 20, 17, 20, 17]
//     Explanation:
//       - SnapshotArray(2) initializes 2 empty logs and snap_id_ = 0; the initial
//         snap() returns 0 and increments snap_id_ to 1 before any writes.
//       - set(1, 17) appends {1, 17} to history_[1] and set(0, 20) appends {1, 20}
//         to history_[0], followed by three snap() calls returning 1, 2, and 3.
//       - For get(0, 0) and get(1, 0), upper_bound(0) returns begin() because all
//         recorded snap_ids are 1 > 0, so both return the default 0.
//       - For get(0, 1), get(1, 1), get(0, 3), and get(1, 3), upper_bound returns
//         end(), so std::prev(it)->value returns 20, 17, 20, and 17.
//
// Algorithm:
//   Per-Index Versioned History with Binary Search:
//   Copying the whole array on every `snap()` requires O(length * snaps) time
//   and space, which overflows memory for `length = 50000`. Instead, store a
//   sparse modification log per index:
//   - `history_[index]` holds `(snap_id, value)` pairs in strictly increasing
//     `snap_id` order.
//   - `set(index, val)`: if the last entry for `index` already belongs to the
//     current `snap_id_`, overwrite its value in place; otherwise append
//     `{snap_id_, val}` in amortized O(1).
//   - `snap()`: increment `snap_id_` and return the previous ID in O(1) — zero
//     copying performed.
//   - `get(index, snap_id)`: binary search `history_[index]` using
//     `std::ranges::upper_bound` projected on `Version::snap_id` to locate the
//     first entry with `entry.snap_id > snap_id`. If that iterator is
//     `begin()`, `index` was never written at or before `snap_id`, so return
//     the default `0`; otherwise return `std::prev(it)->value`.
//   Time Complexity: Constructor O(length), `set` O(1) amortized, `snap` O(1),
//   `get` O(log S) where S is the number of distinct snapshots that wrote to
//   `index`.
//   Space Complexity: O(length + W), where W is the number of `set` calls
//   across distinct snapshots.
//
// Data Structures:
//   - Array of sorted version logs (`std::vector<std::vector<Version>>`):
//     direct O(1) indexing by array position combined with compact per-cell
//     append-only logs supporting O(log S) binary search.
//
// Real-World Applications:
//   - Copy-on-write / persistent data structures in filesystems (ZFS, Btrfs,
//     APFS) and virtual machine disk snapshots.
//   - Database MVCC and time-travel queries (Delta Lake, Apache Iceberg):
//     cheap O(1) snapshot creation with log-time historical cell lookup.
//   - Undo/redo and state-history engines in editors, spreadsheets, and
//     version-controlled configuration stores.
//
// Related LeetCode Problems:
//   - #981 Time Based Key Value Store [done] (same per-key sorted version log + upper_bound lookup)
//   - #146 LRU Cache [done] (stateful data structure design with O(1) updates)
//   - #1472 Design Browser History [done] (versioned navigation history over a dynamic array)
//   - #1622 Fancy Sequence [new] (array state history with lazy versioning)

#include "framework/lc_framework.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace {

class SnapshotArray {
 public:
  // Initializes `length` empty version logs (implicit default value is 0).
  // Time: O(length), Space: O(length).
  explicit SnapshotArray(int length)
      : history_(static_cast<std::size_t>(length)) {}

  // Records `val` at `index` for the current `snap_id_`.
  // Time: O(1) amortized.
  void set(int index, int val) {
    auto& versions = history_[static_cast<std::size_t>(index)];
    // Overwrite in place if `index` was already modified during this snapshot;
    // keeps `versions` strictly increasing by `snap_id` and avoids bloat.
    if (!versions.empty() && versions.back().snap_id == snap_id_) {
      versions.back().value = val;
      return;
    }
    versions.push_back({.snap_id = snap_id_, .value = val});
  }

  // Advances the snapshot counter and returns the newly frozen snapshot ID.
  // Time: O(1).
  int snap() { return snap_id_++; }

  // Returns the value of `index` as of `snap_id`.
  // Time: O(log S), where S is the number of snapshots that modified `index`.
  [[nodiscard]] int get(int index, int snap_id) const {
    const auto& versions = history_[static_cast<std::size_t>(index)];
    // Locate the first write strictly after `snap_id`.
    const auto it = std::ranges::upper_bound(versions, snap_id, {},
                                             &Version::snap_id);
    if (it == versions.begin()) {
      // No write occurred at or before `snap_id`; default initial value is 0.
      return 0;
    }
    return std::prev(it)->value;
  }

 private:
  struct Version {
    int snap_id = 0;
    int value = 0;
  };

  std::vector<std::vector<Version>> history_;
  int snap_id_ = 0;
};

LC_PROBLEM(1146, "Snapshot Array", "hash", ctx) {
  ctx.test("example_1_basic", [&] {
    SnapshotArray arr(3);
    arr.set(0, 5);
    ctx.expect_eq(arr.snap(), 0);
    arr.set(0, 6);
    ctx.expect_eq(arr.get(0, 0), 5);
    ctx.expect_eq(arr.snap(), 1);
    ctx.expect_eq(arr.get(0, 1), 6);
    ctx.expect_eq(arr.get(1, 1), 0);
  });

  ctx.test("python_main_snap_before_set_and_repeated_snaps", [&] {
    SnapshotArray arr(2);
    ctx.expect_eq(arr.snap(), 0);
    arr.set(1, 17);
    arr.set(0, 20);
    ctx.expect_eq(arr.snap(), 1);
    ctx.expect_eq(arr.snap(), 2);
    ctx.expect_eq(arr.snap(), 3);

    ctx.expect_eq(arr.get(0, 0), 0);
    ctx.expect_eq(arr.get(1, 0), 0);
    ctx.expect_eq(arr.get(0, 1), 20);
    ctx.expect_eq(arr.get(1, 1), 17);
    ctx.expect_eq(arr.get(0, 3), 20);
    ctx.expect_eq(arr.get(1, 3), 17);
  });

  ctx.test("multiple_overwrites_within_same_snapshot", [&] {
    SnapshotArray arr(4);
    arr.set(2, 10);
    arr.set(2, 25);
    arr.set(2, 0);
    ctx.expect_eq(arr.snap(), 0);

    arr.set(2, 99);
    arr.set(2, 42);
    ctx.expect_eq(arr.snap(), 1);

    ctx.expect_eq(arr.get(2, 0), 0);
    ctx.expect_eq(arr.get(2, 1), 42);
  });

  ctx.test("reset_to_zero_after_nonzero", [&] {
    SnapshotArray arr(1);
    arr.set(0, 7);
    ctx.expect_eq(arr.snap(), 0);
    arr.set(0, 0);
    ctx.expect_eq(arr.snap(), 1);

    ctx.expect_eq(arr.get(0, 0), 7);
    ctx.expect_eq(arr.get(0, 1), 0);
  });
}

}  // namespace
