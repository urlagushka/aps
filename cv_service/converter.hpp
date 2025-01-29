#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <atomic>
#include <format>
#include <thread>

enum class cv_state_t
{
  FREE,
  READY,
  PROCESS,
  COMPLETE
};

template< typename dtype, typename efunc >
class converter_t
{
  public:
    converter_t(const dtype & dnull);
    void set_item(const dtype & rhs);
    void process_item();
    dtype get_item();
    bool is_available() const;
    bool is_complete() const;

  private:
    const dtype & __dnull;
    dtype __item;
    cv_state_t __state;
};

template< typename dtype, typename efunc >
converter_t< dtype, efunc >::converter_t(const dtype & dnull):
  __dnull(dnull),
  __item(dnull),
  __state(cv_state_t::FREE)
{}

template< typename dtype, typename efunc >
void
converter_t< dtype, efunc >::set_item(const dtype & rhs)
{
  if (__state != cv_state_t::FREE)
  {
    throw std::runtime_error("converter is locked");
  }
  __item = rhs;
  __state = cv_state_t::READY;
}

template< typename dtype, typename efunc >
void
converter_t< dtype, efunc >::process_item()
{
  if (__item == __dnull || __state != cv_state_t::READY)
  {
    throw std::runtime_error(std::format("cannot process, current state: {}", static_cast< int >(__state)));
  }
  
  std::thread([this](){
    std::size_t tmp = efunc()(static_cast< std::size_t >(__item["filesize"]));
    std::this_thread::sleep_for(std::chrono::milliseconds(tmp));
    __state = cv_state_t::COMPLETE;
  }).detach();
}

template< typename dtype, typename efunc >
dtype
converter_t< dtype, efunc >::get_item()
{
  if (__state == cv_state_t::COMPLETE)
  {
    __state = cv_state_t::FREE;
    return __item;
  }
  __state = cv_state_t::FREE;
  return __dnull;
}

template< typename dtype, typename efunc >
bool
converter_t< dtype, efunc >::is_available() const
{
  return __state == cv_state_t::FREE;
}

template< typename dtype, typename efunc >
bool
converter_t< dtype, efunc >::is_complete() const
{
  return __state == cv_state_t::COMPLETE;
}

#endif
