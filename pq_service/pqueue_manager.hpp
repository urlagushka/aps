#ifndef PQUEUE_MANAGER_HPP
#define PQUEUE_MANAGER_HPP

#include <cstddef>

#include "src/pqueue.hpp"

template< typename dtype, typename pred >
class PQueueManager
{
  public:
    PQueueManager() = delete;
    PQueueManager(std::size_t limit, const dtype & dnull);
    dtype add(const dtype & rhs);
    dtype get();
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

#endif
