#ifndef PQUEUE_MANAGER_HPP
#define PQUEUE_MANAGER_HPP

#include <cstddef>
#include <vector>
#include <algorithm>

#include "src/pqueue.hpp"

template< typename dtype, typename pred >
class PQueueManager
{
  public:
    PQueueManager() = delete;
    PQueueManager(std::size_t limit, const dtype & dnull);
    dtype add(const dtype & rhs);
    dtype get();
    void drop();
    const std::vector< dtype > & dump() const;
    const bool is_contains(const dtype & rhs);
    const bool is_empty();

  private:
    pqueue_t< dtype, pred > pqueue_;
};

template< typename dtype, typename pred >
PQueueManager< dtype, pred >::PQueueManager(std::size_t limit, const dtype & dnull):
  pqueue_(limit, dnull)
{}

template< typename dtype, typename pred >
dtype
PQueueManager< dtype, pred >::add(const dtype & rhs)
{
  return pqueue_.push(rhs);
}

template< typename dtype, typename pred >
dtype
PQueueManager< dtype, pred >::get()
{
  return pqueue_.get();
}

template< typename dtype, typename pred >
void
PQueueManager< dtype, pred >::drop()
{
  pqueue_.drop();
}

template< typename dtype, typename pred >
const std::vector< dtype > &
PQueueManager< dtype, pred >::dump() const
{
  return pqueue_.dump();
}

template< typename dtype, typename pred >
const bool
PQueueManager< dtype, pred >::is_contains(const dtype & rhs)
{
  const auto & tmp = dump();
  return std::find(tmp.begin(), tmp.end(), rhs) != tmp.end();
}

template< typename dtype, typename pred >
const bool
PQueueManager< dtype, pred >::is_empty()
{
  return pqueue_.size() == 0;
}

#endif
