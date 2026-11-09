#pragma once

#include <cstdint>
#include <iterator>
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

  // iterator too pool items. not guarenteed to be in insertion order
  class Iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

  private:
    typename std::vector<Item>::iterator *ptr_, *end_;

    explicit Iterator(
        typename std::vector<Item>::iterator ptr, typename std::vector<Item>::iterator end
    ) : ptr_(ptr), end_(end) {
      skip_free();
    }
    friend class Pool;

    void skip_free() {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      for (; ptr_ != end_ && ptr_->is_free; ++ptr_)
        ;
    }

  public:
    reference operator*() const { return ptr_->val; }
    pointer operator->() const { return ptr_->val; }

    Iterator &operator++() {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      ++ptr_;
      skip_free();
      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const Iterator &a, const Iterator &b) { return a.ptr_ == b.ptr_; }
    friend bool operator!=(const Iterator &a, const Iterator &b) { return !(a == b); }
  };

  Iterator begin() { return Iterator(pool_.begin(), pool_.end()); }
  Iterator end() { return Iterator(pool_.end(), pool_.end()); }
};

} // namespace fysics::detail
