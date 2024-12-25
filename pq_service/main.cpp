#include "src/http/server.hpp"

#include <stdexcept>
#include <iostream>

int main()
{
  try
  {
    http::server_h server(8080);

    server.post("/add", [](const nlohmann::json & rhs) -> nlohmann::json
    {
      std::cout << rhs.dump(2) << std::endl;
      return nlohmann::json::parse("{}");
    });

    server.post("/status", [](const nlohmann::json & rhs) -> nlohmann::json
    {
      std::cout << rhs.dump(2) << std::endl;
      return nlohmann::json::parse("{}");
    });

    server.start();
    while (true);
  }
  catch (const std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
  }
}
