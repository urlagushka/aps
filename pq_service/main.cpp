#include <stdexcept>
#include <iostream>
#include <functional>

#include "src/http/server.hpp"
#include "pqueue_manager.hpp"

struct pred_t
{
  bool operator()(const nlohmann::json &)
  {
    return true;
  }
};

int main()
{
  nlohmann::json dnull;
  PQueueManager< nlohmann::json, pred_t > pqm(10, dnull);

  try
  {
    http::server_h server(8080);

    server.post("/add",
    [dnull, &pqm](const nlohmann::json & rhs) -> nlohmann::json
    {
      nlohmann::json response;
      try
      {
        auto invalid = pqm.add(rhs);
        if (invalid == dnull)
        {
          response["status"] = 1;
          response["invalid"] = 0;
          return response;
        }
        response["status"] = 0;
        return response;
      }
      catch (const std::runtime_error & error)
      {
        response["status"] = 2;
        response["error"] = error.what();
        return response;
      }
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
