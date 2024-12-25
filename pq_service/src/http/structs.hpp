#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <optional>
#include <string>
#include "json.hpp"

namespace http
{
  struct response_t
  {
    std::string httpv;
    std::size_t status_code;
    std::string reason_phrase;

    std::optional< std::string > ctype;
    std::optional< std::size_t > csize;
    std::optional< nlohmann::json > body;

    std::string dump() const;
  };

  struct request_t
  {
    // all
    std::string req;
    std::string endpoint;
    std::string httpv;
    std::string host;

    // post
    std::optional< std::string > useragent;
    std::optional< std::string > ctype;
    std::optional< std::size_t > csize;
    std::optional< nlohmann::json > body;

    request_t & operator=(const std::string & rhs);
  };
}

#endif
