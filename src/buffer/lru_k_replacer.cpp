//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"
#include <cstddef>
#include <optional>
#include <ostream>
#include "common/exception.h"

namespace bustub {

/**
 *
 * TODO(P1): Add implementation
 *
 * @brief a new LRUKReplacer.
 * @param num_frames the maximum number of frames the LRUReplacer will be required to store
 */
LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}
void PrintNodeStore(const std::unordered_map<frame_id_t, LRUKNode> &node_store) {
  for (const auto &[fid, node] : node_store) {
    std::cout << "Frame ID: " << fid << "\n";
    std::cout << "  k: " << node.k_ << "\n";
    std::cout << "  is_evictable: " << (node.is_evictable_ ? "true" : "false") << "\n";
    std::cout << "  History timestamps: ";
    for (size_t ts : node.history_) {
      std::cout << ts << " ";
    }
    std::cout << "\n\n";
  }
}
/**
 * TODO(P1): Add implementation
 *
 * @brief Find the frame with largest backward k-distance and evict that frame. Only frames
 * that are marked as 'evictable' are candidates for eviction.
 *
 * A frame with less than k historical references is given +inf as its backward k-distance.
 * If multiple frames have inf backward k-distance, then evict frame whose oldest timestamp
 * is furthest in the past.
 *
 * Successful eviction of a frame should decrement the size of replacer and remove the frame's
 * access history.
 *
 * @return the frame ID if a frame is successfully evicted, or `std::nullopt` if no frames can be evicted.
 */
auto LRUKReplacer::Evict() -> std::optional<frame_id_t> {
  //PrintNodeStore(node_store_);
  size_t oldest_time = std::numeric_limits<unsigned long>::max();
  size_t biggest_k = 0;
  frame_id_t oldest_time_id = -1;
  frame_id_t biggest_k_id = -1;
  for (const auto &[frame_id, node] : node_store_) {
    if (node.history_.size() < k_) {
      auto last_time_entry = node.history_.back();
      if (last_time_entry < oldest_time && node.is_evictable_) {
        oldest_time = last_time_entry;
        oldest_time_id = frame_id;
        // std::cout << "Oldest Node: " << frame_id << std::endl;
      }
      continue;
    }
    auto time_entry_at_k = *std::prev(node.history_.end(), k_);
    // std::cout << "Checking Node: " << frame_id << " time stamp at k" << time_entry_at_k << " Biggest K:" << biggest_k << std::endl;
    auto time_entry_at_latest = NowNanos();
    if (time_entry_at_latest - time_entry_at_k > biggest_k && node.is_evictable_) {
      biggest_k = time_entry_at_latest - time_entry_at_k;
      biggest_k_id = frame_id;
    //   std::cout << "Biggest K: " << frame_id << std::endl;
    }
  }
  if (oldest_time_id != -1) {
    Remove(oldest_time_id);
    return oldest_time_id;
  } else if (biggest_k_id != -1) {
    Remove(biggest_k_id);
    return biggest_k_id;
  }
  return std::nullopt;
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Record the event that the given frame id is accessed at current timestamp.
 * Create a new entry for access history if frame id has not been seen before.
 *
 * If frame id is invalid (ie. larger than replacer_size_), throw an exception. You can
 * also use BUSTUB_ASSERT to abort the process if frame id is invalid.
 *
 * @param frame_id id of frame that received a new access.
 * @param access_type type of access that was received. This parameter is only needed for
 * leaderboard tests.
 */
void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {
  auto it = node_store_.find(frame_id);
  if (it != node_store_.end()) {
    LRUKNode &node = it->second;
    auto now = NowNanos();
    node.history_.push_back(now);
  } else {
    node_store_.try_emplace(frame_id, LRUKNode(k_, frame_id));
    // std::cout << "Node not found for frame_id: " << frame_id << " record access for now: " << NowNanos() << std::endl;
  }
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Toggle whether a frame is evictable or non-evictable. This function also
 * controls replacer's size. Note that size is equal to number of evictable entries.
 *
 * If a frame was previously evictable and is to be set to non-evictable, then size should
 * decrement. If a frame was previously non-evictable and is to be set to evictable,
 * then size should increment.
 *
 * If frame id is invalid, throw an exception or abort the process.
 *
 * For other scenarios, this function should terminate without modifying anything.
 *
 * @param frame_id id of frame whose 'evictable' status will be modified
 * @param set_evictable whether the given frame is evictable or not
 */
void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  auto it = node_store_.find(frame_id);
  if (it != node_store_.end()) {
    LRUKNode &node = it->second;
    //std::cout << node.fid_ << " set evictable" << std::endl;
    node.is_evictable_ = set_evictable;
  }
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Remove an evictable frame from replacer, along with its access history.
 * This function should also decrement replacer's size if removal is successful.
 *
 * Note that this is different from evicting a frame, which always remove the frame
 * with largest backward k-distance. This function removes specified frame id,
 * no matter what its backward k-distance is.
 *
 * If Remove is called on a non-evictable frame, throw an exception or abort the
 * process.
 *
 * If specified frame is not found, directly return from this function.
 *
 * @param frame_id id of frame to be removed
 */
void LRUKReplacer::Remove(frame_id_t frame_id) {
  auto it = node_store_.find(frame_id);
  if (it != node_store_.end()) {
    LRUKNode &node = it->second;
    if (node.is_evictable_ == true) {
    //   std::cout << "removing frame : " << frame_id << std::endl;
      node_store_.erase(frame_id);
    } else {
      throw bustub::Exception("Node is not evictable");
    }
  }
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Return replacer's size, which tracks the number of evictable frames.
 *
 * @return size_t
 */
auto LRUKReplacer::Size() -> size_t {
  auto size = 0;
  for (const auto &[frame_id, node] : node_store_) {
    if (node.is_evictable_ == true) {
      size++;
    }
    // std::cout << "Frame ID: " << frame_id << " evictable" << std::endl;
  }
  return size;
}

}  // namespace bustub
