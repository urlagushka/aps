#ifndef PQUEUE_HPP
#define PQUEUE_HPP

#include <queue>

template< typename type, typename pred >
class PQueue
{
    public:
        PQueue() = delete;
        PQueue(std::size_t limit);

        const bool is_available() const
        const bool push(const type & rhs);
        type get() const;
        void drop();
    private:
        std::priority_queue< type, pred > pqueue_;
        std::size_t limit_;
};

template< typename type, typename pred >
PQueue::PQueue(std::size_t limit):
    limit_(limit)
{}

template< typename type, typename pred >
const bool PQueue::is_available() const
{
    return pqueue_.size() < limit_;
}

template< typename type, typename pred >
const bool PQueue::push(const type & rhs)
{
    if (!is_available())
    {
        return false;
    }
    pqueue_.push(rhs);
    return true;
}

template< typename type, typename pred >
PQueue::type PQueue::get() const
{
    return pqueue_.top();
}

template< typename type, typename pred >
void PQueue::drop()
{
    return pqueue_.pop();
}

#endif
