#include "request.hpp"

const request_t make_request(const client & cl, const std::string & path)
{
  const file_fd fl = get_file(path);

  return {cl, fl};
}
