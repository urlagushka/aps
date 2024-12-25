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

    const dtype & push(const dtype & rhs);
    const dtype & get() const;
    void drop();

    const bool is_available() const;
    const std::size_t size() const;

  private:
    std::pair< iter_t, const dtype & > swap_by_pred(const dtype & rhs);
    c_iter_t get_by_pred() const;
    const dtype & shift();

    vec_t queue_;
    const dtype & dnull_;
    std::size_t limit_;
};

template< typename dtype, typename pred >
pqueue_t< dtype, pred >::pqueue_t(std::size_t limit, const dtype & dnull):
  queue_(limit),
  dnull_(dnull),
  limit_(limit)
{}

template< typename dtype, typename pred >
const dtype &
pqueue_t< dtype, pred >::push(const dtype & rhs)
{
  if (queue_.size() == limit_)
  {
    std::pair< iter_t, const dtype & > ret = swap_by_pred(rhs);
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
    throw std::runtime_error("queue is empty, cannot get!");
  }
  return queue_[0];
}

template< typename dtype, typename pred >
void
pqueue_t< dtype, pred >::drop()
{
  if (size() == 0)
  {
    throw std::runtime_error("queue is empty, cannot drop!");
  }

  shift();
}

template< typename dtype, typename pred >
const bool
pqueue_t< dtype, pred >::is_available() const
{
  return size() < limit_;
}

template< typename dtype, typename pred >
const std::size_t
pqueue_t< dtype, pred >::size() const
{
  return queue_.size();
}
 
template< typename dtype, typename pred >
std::pair< typename pqueue_t< dtype, pred >::iter_t, const dtype & >
pqueue_t< dtype, pred >::swap_by_pred(const dtype & rhs)
{
  c_iter_t found = get_by_pred();
  if (found == queue_.cend())
  {
    return std::make_pair(queue_.end(), dnull_);
  }

  iter_t ret = queue_.erase(found, found);
  const dtype & tmp = *ret;
  *ret = rhs;
  return std::make_pair(ret, tmp);
}

template< typename dtype, typename pred >
pqueue_t< dtype, pred >::c_iter_t
pqueue_t< dtype, pred >::get_by_pred() const
{
  c_iter_t ret = queue_.cend();
  for (c_iter_t it = queue_.cbegin(); it != queue_.cend(); ++it)
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
  vec_t tmp(limit_);
  std::copy(++queue_.cbegin(), queue_.cend(), std::back_inserter(tmp));
  queue_ = tmp;
  return ret;
}

#endif
