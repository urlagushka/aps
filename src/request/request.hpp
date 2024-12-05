#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "file.hpp"
#include "client.hpp"

struct request_t
{
  const client & client;
  const file_fd file;
};

const request_t make_request(const client & cl, const std::string & fl);

#endif
