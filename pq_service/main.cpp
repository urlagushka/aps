#include <stdexcept>
#include <iostream>
#include <functional>
#include <format>
#include <thread>
#include <chrono>

#include "src/request/curl_handler.hpp"
#include "src/http/server.hpp"
#include "pqueue_manager.hpp"

struct pred_t
{
  bool operator()(const nlohmann::json & rhs)
  {
    return rhs["premium"] == 0;
  }
};

int main()
{
  nlohmann::json dnull;
  PQueueManager< nlohmann::json, pred_t > * pqm = nullptr;
  std::mutex pqm_mutex;
  nlohmann::json items = {
    {"items", nlohmann::json::array()}
  };
  std::atomic< bool > is_send = true;
 
  try
  {
    http::server_h server(8085);

    server.post("/start",
    [dnull, &pqm, &pqm_mutex, &is_send](const nlohmann::json & rhs) -> nlohmann::json
    {
      std::lock_guard< std::mutex > lock(pqm_mutex);
      try
      {
        int size = rhs["pqm_size"];
        if (pqm != nullptr)
        {
          delete pqm;
          pqm = nullptr;
        }
        pqm = new PQueueManager< nlohmann::json, pred_t >(size, dnull);
        is_send = true;
        
        std::cout << std::format("INFO | pqm created with size {}", size) << std::endl;
      }
      catch (const std::exception & error)
      {
        std::cout << error.what() << std::endl;
      }
      nlohmann::json response;
      return response;
    });

    server.post("/stop",
    [dnull, &pqm, &pqm_mutex, &is_send](const nlohmann::json & rhs) -> nlohmann::json
    {
      is_send = true;
      std::lock_guard< std::mutex > lock(pqm_mutex);
      if (pqm != nullptr)
      {
        delete pqm;
        pqm = nullptr;
      }

      std::cout << "INFO | pqm deleted" << std::endl;
      nlohmann::json response;
      return response;
    });

    server.post("/add",
    [dnull, &pqm, &pqm_mutex](const nlohmann::json & rhs) -> nlohmann::json
    {
      std::lock_guard< std::mutex > lock(pqm_mutex);
      nlohmann::json response;
      int nid = rhs["id"];
      try
      {
        auto item = pqm->add(rhs);
        if (item == dnull)
        {
          response["status"] = 0;
          response["item"] = 0;
          std::cout << std::format("INFO | client#{} added, no drop", nid) << std::endl;
          return response;
        }

        int oid = item["id"];
        response["status"] = 1;
        item["status"] = "отмена";
        response["item"] = item;
        std::cout << std::format("INFO | client#{} added, client#{} dropped", nid, oid) << std::endl;
      }
      catch (const std::runtime_error & error)
      {
        response["status"] = 2;
        response["error"] = error.what();
        std::cout << std::format("ERROR | client#{} add error: {}", nid, error.what()) << std::endl;
      }
      
      return response;
    });

    server.post("/update_cls",
    [dnull, &pqm, &pqm_mutex, &items](const nlohmann::json &) -> nlohmann::json
    {
      std::lock_guard< std::mutex > lock(pqm_mutex);
      nlohmann::json response = items;
      items["items"] = nlohmann::json::array();
      response["status"] = 0;
      return response;
    });

    server.post("/continue",
    [&is_send](const nlohmann::json &) -> nlohmann::json
    {
      is_send = true;
      nlohmann::json response;
      return response;
    });

    server.start();

    curl::curl_handler qq("pq_service");

    while (true)
    {
      std::lock_guard< std::mutex > lock(pqm_mutex);
      if (pqm == nullptr)
      {
        continue;
      }

      try
      {
        auto item = pqm->get();
        if (item != dnull && is_send)
        {
          nlohmann::json response = qq.async_post< nlohmann::json >("http://localhost:8086/add", item).get();
          if (response.contains("status"))
          {
            if (response["status"] == 0)
            {
              pqm->drop();
              item["status"] = "обработка";
              items["items"].push_back(item);
            }
            else if (response["status"] == 1 && response.contains("error"))
            {
              std::string tmp = response["error"];
              std::cout << std::format("ERROR | cvv: {}", tmp) << std::endl;
              is_send = false;
            }
          }
        }
      }
      catch (const std::runtime_error & error)
      {
        std::cerr << "un error | " << error.what() << std::endl;
      }
    }
  }
  catch (const std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
  }
}
