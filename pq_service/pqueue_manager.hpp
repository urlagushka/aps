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
    const std::vector< dtype > & dump() const;
    const bool is_contains(const dtype & rhs);

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
  if (!pqueue_.is_available())
  {
    throw std::runtime_error("queue is full!");
  }
  return pqueue_.push(rhs);
}

template< typename dtype, typename pred >
dtype
PQueueManager< dtype, pred >::get()
{
  auto tmp = pqueue_.get();
  pqueue_.drop();
  return tmp;
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

#endif
