#include "client.hpp"

client::client(std::size_t id, user_level_t level):
  id_(id),
  level_(level)
{}

const std::size_t client::get_id() const
{
  return id_;
}

const user_level_t client::get_level() const
{
  return level_;
}
