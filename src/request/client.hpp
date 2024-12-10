#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstddef>

enum class user_level_t
{
  PREMIUM,
  COMMON
};

class client_t
{
  public:
    client() = delete;
    client(std::size_t id, user_level_t level);

    const std::size_t get_id() const;
    const user_level_t get_level() const;

  private:
    std::size_t id_;
    user_level_t level_;
};

#endif
