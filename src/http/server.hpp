#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstddef>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>

#include "structs.hpp"

namespace http
{
  class server_h
  {
    using addf_t = void (*)(const request_t &, response_t &);
    public:
      server_h(std::size_t port);
      ~server_h();

      void post(const std::string & endpoint, addf_t addf);
      void get(const std::string & endpoint, addf_t addf);

      void start();
      void stop();

    private:
      void process(int client);

      std::thread __server;
      std::atomic< bool > __flag;
      std::mutex __post_mutex;
      std::mutex __get_mutex;

      std::unordered_map< std::string, addf_t > __post;
      std::unordered_map< std::string, addf_t > __get;

      int __socket;
  };
}

#endif
