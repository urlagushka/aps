#include "structs.hpp"

#include <cstring>
#include <iostream>
#include <format>

http::request_t &
http::request_t::operator=(const std::string & rhs)
{
  std::cout << rhs << std::endl;
  std::istringstream stream(rhs);
  std::string line;

  std::getline(stream, line);
  std::istringstream(line) >> req >> endpoint >> httpv;

  while (std::getline(stream, line) && !line.empty())
  {
    auto pos = line.find(':');
    if (pos != std::string::npos)
    {
      std::string key = line.substr(0, pos);
      std::string value = line.substr(pos + 1);
      value.erase(0, value.find_first_not_of(" \t"));
      value.erase(value.find_last_not_of(" \t\r\n") + 1);

      if (key == "Host")
      {
        host = value;
      }
      else if (key == "User-Agent")
      {
        useragent = value;
      }
      else if (key == "Content-Type")
      {
        ctype = value;
      }
      else if (key == "Content-Length")
      {
        csize = std::stoul(value);
      }
    }
  }

  if (ctype.has_value() && ctype.value() == "application/json")
  {
    std::string body_content = rhs.substr(rhs.size() - csize.value(), rhs.size());
    std::cout << body_content << std::endl;
    if (!body_content.empty())
    {
      try
      {
        body = nlohmann::json::parse(body_content);
      }
      catch (...)
      {
        throw std::runtime_error(std::format("parsing error: {}", body_content));
      }
    }
  }

  return * this;
}