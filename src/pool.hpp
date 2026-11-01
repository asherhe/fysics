#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "fysics/types.hpp"

namespace fysics::detail {

// int type for indexing Pool elements. Pools cannot hold more items than this type's max value
using PoolIndex = uint32_t;
// default capacity of pool on startup
constexpr PoolIndex DEFAULT_POOL_CAPACITY = 128;

// object pool container that stores items contiguously in memory for indexing. uses generational
// indices to account for object lifecycles safely
template <typename T> class Pool {
private:
  // keeps track of additional pool item metadata alongside stored value
  struct Item {
    bool is_free = false;      // whether this is available for allocation
    Generation generation = 0; // how many times this item has already been allocated
    T val;                     // stored value
  };
  std::vector<Item> pool_;
  // stack of available slots in the pool
  std::vector<PoolIndex> free_ids_;

public:
  Pool(PoolIndex capacity = DEFAULT_POOL_CAPACITY) { pool_.reserve(capacity); }

  // unique identifier for an item in the pool.
  struct Id {
    PoolIndex id;
    Generation generation;
  };

  // find storage for a new item, set it to val, and return a Pool::Id for that new item
  Id add(const T &val) {
    if (free_ids_.empty()) {
      Id id{static_cast<PoolIndex>(pool_.size()), 0};
      pool_.push_back({false, 0, val});
      return id;
    }
    Id id{free_ids_.back(), 0};
    id.generation = ++pool_[id.id].generation;
    free_ids_.pop_back();
    pool_[id.id].is_free = false;
    pool_[id.id].val = val;
    return id;
  }

  // retrieve an item from the pool. does NOT check if the id is valid.
  T &get(Id id) noexcept { return pool_[id.id].val; }

  // retrieve an item from the pool. throws an exception if the given id is invalid
  T &at(Id id) {
    if (!is_valid(id))
      throw std::out_of_range("Pool: invalid or stale id");
    return get(id);
  }

  // release an item from the pool. invalidates the id of that item
  void erase(Id id) {
    if (!is_valid(id))
      return;
    pool_[id.id].is_free = true;
    free_ids_.push_back(id.id);
  }

  // an id is only valid if its storage is currently allocated and it matches the current generation
  bool is_valid(Id id) const {
    return id.id < pool_.size() && !pool_[id.id].is_free &&
           pool_[id.id].generation == id.generation;
  }
};

} // namespace fysics::detail
