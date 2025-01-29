#include <stdexcept>
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>

#include "src/request/curl_handler.hpp"
#include "src/http/server.hpp"
#include "converter.hpp"

struct wait_t
{
  double operator()(std::size_t size)
  {
    return size * 2.17;
  }
};

int main()
{
  using cv_t = converter_t< nlohmann::json, wait_t >;
  std::vector< cv_t > * cv_vec = nullptr;
  std::mutex cv_mutex;
  std::size_t dnull = 0;
  std::atomic< bool > is_send = true;
  nlohmann::json items = {
    {"items", nlohmann::json::array()}
  };
  nlohmann::json items_cl = {
    {"items", nlohmann::json::array()}
  };

  try
  {
    http::server_h server(8086);

    server.post("/start",
    [&cv_vec, &cv_mutex, &dnull, &is_send](const nlohmann::json & rhs) -> nlohmann::json
    {
      std::lock_guard< std::mutex > lock(cv_mutex);
      nlohmann::json response;

      if (cv_vec != nullptr)
      {
        delete cv_vec;
        cv_vec = nullptr;
      }
      int size = rhs["cvv_size"];
      if (size < 0)
      {
        std::cout << std::format("WARNING | cvv size < 0, size is {}", size) << std::endl;
        response["status"] = 1;
        response["error"] = std::format("cvv size < 0, size is {}", size);
        return response;
      }

      cv_vec = new std::vector< cv_t >(size, cv_t(dnull));
      is_send = true;
      std::cout << std::format("INFO | cvv created with size {}", size) << std::endl;
      response["status"] = 0;
      return response;
    });

    server.post("/stop",
    [&cv_vec, &cv_mutex, &is_send](const nlohmann::json & rhs) -> nlohmann::json
    {
      is_send = true;
      std::lock_guard< std::mutex > lock(cv_mutex);
      nlohmann::json response;
      delete cv_vec;
      cv_vec = nullptr;
      response["status"] = 0;
      std::cout << "INFO | cvv deleted" << std::endl;
      return response;
    });

    server.post("/add",
    [&cv_vec, &cv_mutex, &is_send, &items](const nlohmann::json & rhs) -> nlohmann::json
    {
      std::lock_guard< std::mutex > lock(cv_mutex);
      nlohmann::json response;

      if (cv_vec == nullptr)
      {
        std::cout << "ERROR | cvv is null" << std::endl;
        response["status"] = 1;
        response["error"] = "cvv is null";
        return response;
      }

      auto tmp = std::find_if(cv_vec->begin(), cv_vec->end(), [](const cv_t & rhs)
      {
        return rhs.is_available();
      });
      if (tmp == cv_vec->end())
      {
        std::cout << "WARNING | no available converters" << std::endl;
        response["status"] = 1;
        response["error"] = "no available converters";
        is_send = false;
        return response;
      }

      try
      {
        tmp->set_item(rhs);
        tmp->process_item();
        nlohmann::json cvn = {
          {"id", std::distance(cv_vec->begin(), tmp) + 1},
          {"filename", rhs["filename"]},
          {"filesize", rhs["filesize"]},
          {"source_fmt", rhs["source_fmt"]},
          {"target_fmt", rhs["target_fmt"]},
          {"status", "занят"}
        };
        items["items"].push_back(cvn);
        response["status"] = 0;
        std::cout << "INFO | cvv item added" << std::endl;
      }
      catch (const std::runtime_error & error)
      {
        std::cout << std::format("ERROR | cvv item process error: {}", error.what()) << std::endl;
        response["status"] = 1;
        response["error"] = error.what();
      }

      return response;
    });

    server.post("/update_cvs",
    [&cv_vec, &cv_mutex, &items](const nlohmann::json &) -> nlohmann::json
    {
      std::lock_guard< std::mutex > lock(cv_mutex);
      nlohmann::json response = items;
      items["items"] = nlohmann::json::array();
      response["status"] = 0;
      return response;
    });

    server.post("/update_cls",
    [&cv_vec, &cv_mutex, &items_cl](const nlohmann::json &) -> nlohmann::json
    {
      std::lock_guard< std::mutex > lock(cv_mutex);
      nlohmann::json response = items_cl;
      items_cl["items"] = nlohmann::json::array();
      response["status"] = 0;
      return response;
    });

    server.start();

    curl::curl_handler qq("cv_service");

    while (true)
    {
      std::lock_guard< std::mutex > lock(cv_mutex);
      if (!is_send)
      {
        auto tmp = std::find_if(cv_vec->begin(), cv_vec->end(), [](const cv_t & rhs)
        {
          return rhs.is_available();
        });
        if (tmp != cv_vec->end())
        {
          qq.post< nlohmann::json >("http://localhost:8085/continue", {});
          is_send = true;
        }
      }
      if (cv_vec == nullptr)
      {
        continue;
      }
      for (std::size_t i = 0; i < cv_vec->size(); ++i)
      {
        if ((*cv_vec)[i].is_complete())
        {
          auto item = (*cv_vec)[i].get_item();

          item["status"] = "готов";
          items_cl["items"].push_back(item);

          item["id"] = i + 1;
          item["status"] = "свободен";
          items["items"].push_back(item);
        }
      }
    }
  }
  catch (const std::runtime_error & error)
  {
    std::cerr << error.what() << std::endl;
  }
}
