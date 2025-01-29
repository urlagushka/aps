#ifndef PQUEUE_HPP
#define PQUEUE_HPP

#include <vector>
#include <stdexcept>
#include <utility>
#include <algorithm>

template< typename dtype, typename pred >
class pqueue_t
{
  using vec_t = std::vector< dtype >;
  using iter_t = vec_t::iterator;
  using c_iter_t = vec_t::const_iterator;

  public:
    pqueue_t() = delete;
    pqueue_t(std::size_t limit, const dtype & dnull);
    ~pqueue_t();

    dtype push(const dtype & rhs);
    const dtype & get() const;
    const dtype & drop();
    const vec_t & dump() const;

    bool is_available() const;
    std::size_t size() const;

  private:
    std::pair< iter_t, dtype > swap_by_pred(const dtype & rhs);
    iter_t get_by_pred();
    const dtype & shift();

    vec_t queue_;
    const dtype & dnull_;
    std::size_t limit_;
};

template< typename dtype, typename pred >
pqueue_t< dtype, pred >::pqueue_t(std::size_t limit, const dtype & dnull):
  queue_(),
  dnull_(dnull),
  limit_(limit)
{}

template< typename dtype, typename pred >
pqueue_t< dtype, pred >::~pqueue_t()
{
  queue_.clear();
}

template< typename dtype, typename pred >
dtype
pqueue_t< dtype, pred >::push(const dtype & rhs)
{
  if (queue_.size() == limit_)
  {
    std::pair< iter_t, dtype > ret = swap_by_pred(rhs);
    if (ret.first == queue_.end())
    {
      throw std::runtime_error("queue is full, cannot swap!");
    }
    return ret.second;
  }

  queue_.push_back(rhs);
  return dnull_;
}

template< typename dtype, typename pred >
const dtype &
pqueue_t< dtype, pred >::get() const
{
  if (size() == 0)
  {
    return dnull_;
  }
  return queue_.front();
}

template< typename dtype, typename pred >
const dtype &
pqueue_t< dtype, pred >::drop()
{
  if (size() == 0)
  {
    return dnull_;
  }

  return shift();
}

template< typename dtype, typename pred >
bool
pqueue_t< dtype, pred >::is_available() const
{
  return size() < limit_;
}

template< typename dtype, typename pred >
std::size_t
pqueue_t< dtype, pred >::size() const
{
  return queue_.size();
}
 
template< typename dtype, typename pred >
std::pair< typename pqueue_t< dtype, pred >::iter_t, dtype >
pqueue_t< dtype, pred >::swap_by_pred(const dtype & rhs)
{
  iter_t found = get_by_pred();
  if (found == queue_.end())
  {
    return std::make_pair(queue_.end(), dnull_);
  }

  dtype tmp = *found;
  *found = rhs;
  return std::make_pair(found, tmp);
}

template< typename dtype, typename pred >
pqueue_t< dtype, pred >::iter_t
pqueue_t< dtype, pred >::get_by_pred()
{
  iter_t ret = queue_.end();
  for (iter_t it = queue_.begin(); it != queue_.end(); ++it)
  {
    if (pred()(*it))
    {
      ret = it;
    }
  }
  return ret;
}

template< typename dtype, typename pred >
const dtype &
pqueue_t< dtype, pred >::shift()
{
  const dtype & ret = get();
  queue_.erase(queue_.begin());
  return ret;
}

template< typename dtype, typename pred >
const pqueue_t< dtype, pred >::vec_t &
pqueue_t< dtype, pred >::dump() const
{
  return queue_;
}

#endif
