#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "file.hpp"
#include "client.hpp"

struct request_t
{
  const client_t & client;
  const file_fd file;
};

const request_t make_request(const client_t & cl, const std::string & fl);

#endif
