#include "src/http/server.hpp"

#include <stdexcept>
#include <iostream>

int main()
{
  try
  {
    http::server_h server(8080);
    server.start();
    while (true);
  }
  catch (const std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
  }
}
