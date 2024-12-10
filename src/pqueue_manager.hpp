#ifndef PQUEUE_MANAGER_HPP
#define PQUEUE_MANAGER_HPP

#include <cstddef>

#include "pqueue.hpp"

template< typename type, typename pred >
class PQueueManager
{
    public:
        PQueueManager() = delete;
        PQueueManager(std::size_t limit);
        constexpr void add(const type & rhs);
        type get();
    private:
        pqueue_t< type, pred > pqueue_;
};

template< typename type, typename pred >
PQueueManager::PQueueManager(std::size_t limit):
    pqueue_(limit)
{}

template< typename type, typename pred >
void PQueueManager::add(const type & rhs)
{
    if (!pqueue_.is_available())
    {
        throw std::runtime_error("queue is full!")
    }
    try
    {
        pqueue_.push(rhs);
    }
    return 
}

template< typename type, typename pred >
PQueueManager::type get()

#endif
