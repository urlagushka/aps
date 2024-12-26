#include <stdexcept>
#include <iostream>
#include <functional>

#include "src/request/curl_handler.hpp"
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
  // SERVER INIT
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

    server.post("/status",
    [dnull, &pqm](const nlohmann::json &) -> nlohmann::json
    {
      nlohmann::json response;
      response["items"] = nlohmann::json::array();
      const auto & tmp = pqm.dump();
      for (const auto & data : tmp)
      {
        std::cout << data << std::endl;
        response["items"].push_back(data);
      }
      return response;
    });

    server.post("/check",
    [dnull, &pqm](const nlohmann::json & rhs) -> nlohmann::json
    {
      nlohmann::json response;
      response["is_contains"] = (pqm.is_contains(rhs)) ? 1 : 0;
      return response;
    });

    server.start();

    // CURL INIT
    curl::curl_handler qq("pq_service");

    // MAIN
    while (true)
    {
      try
      {
        nlohmann::json response;
        auto item = pqm.get();
        do
        {
          response = qq.post< nlohmann::json >("http://localhost:8081/add", item);
        }
        while (response["status"] != 0);
      }
      catch (const std::runtime_error & error)
      {
        std::cerr << error.what() << std::endl;
      }
    }
  }
  catch (const std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
  }
}
