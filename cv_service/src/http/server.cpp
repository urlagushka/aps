#include "server.hpp"

#include <stdexcept>
#include <format>
#include <iostream>
#include <string_view>
#include <netinet/in.h>
#include <functional>

#include "structs.hpp"

http::server_h::server_h(std::size_t port):
  __server(),
  __flag(true),
  __post_mutex(),
  __get_mutex(),
  __post(0),
  __get(0),
  __socket(-1)
{
  __socket = socket(AF_INET, SOCK_STREAM, 0);
  if (__socket == -1)
  {
    throw std::runtime_error(std::format("socket init error: {}", port));
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(__socket, (sockaddr*)&addr, sizeof(addr)) == -1)
  {
    close(__socket);
    throw std::runtime_error(std::format("socket bind error: {}", port));
  }

  if (listen(__socket, 10) == -1)
  {
    close(__socket);
    throw std::runtime_error(std::format("socket listen error: {}", port));
  }
}

http::server_h::~server_h()
{
  close(__socket);
}

void
http::server_h::post(const std::string & endpoint, addf_t addf)
{
  std::lock_guard< std::mutex > lock(__post_mutex);
  __post[endpoint] = addf;
}

void
http::server_h::get(const std::string & endpoint, addf_t addf)
{
  std::lock_guard< std::mutex > lock(__get_mutex);
  __get[endpoint] = addf;
}

void
http::server_h::start()
{
  __server = std::thread([this](){
    while (__flag)
    {
      int client = accept(__socket, nullptr, nullptr);
      if (client < 0)
      {
        continue;
      }

      std::thread(&server_h::process, this, client).detach();
    }
  });
}

void
http::server_h::stop()
{
  __flag = false;
  if (__server.joinable())
  {
    __server.join();
  }
}

void
http::server_h::process(int client)
{
  char buffer[4096] = {0};
  ssize_t bytes_read = read(client, buffer, sizeof(buffer) - 1);
  if (bytes_read <= 0) {
      close(client);
      return;
  }

  response_t response;
  request_t request;
  try
  {
    request = buffer;

    // CORS HEADERS
    std::string cors_headers = 
      "Access-Control-Allow-Origin: *\r\n"
      "Access-Control-Allow-Methods: POST, OPTIONS\r\n"
      "Access-Control-Allow-Headers: Content-Type\r\n";

    if (request.req == "OPTIONS")
    {
      response = {
        .httpv = request.httpv,
        .status_code = 204,
        .reason_phrase = "No Content",
        .headers = cors_headers
      };
    }
    else if (request.req == "POST")
    {
      std::lock_guard< std::mutex > lock(__post_mutex);
      json data = __post.at(request.endpoint)(request.body);
      std::string dump = data.dump();
      response = {
        .httpv = request.httpv,
        .status_code = 200,
        .reason_phrase = "Ok",
        .headers = cors_headers,
        .ctype = request.ctype,
        .csize = dump.size(),
        .body = dump
      };
    }
    else
    {
      response = {
        .httpv = request.httpv,
        .status_code = 400,
        .reason_phrase = "Bad Request",
        .headers = cors_headers
      };
    }
  }
  catch (const std::runtime_error & error)
  {
    response = {
      .httpv = request.httpv,
      .status_code = 500,
      .reason_phrase = error.what()
    };
  }
  catch (const std::out_of_range &)
  {
    response = {
      .httpv = request.httpv,
      .status_code = 404,
      .reason_phrase = "Not Found"
    };
  }
  catch (const std::bad_function_call & error)
  {
    response = {
      .httpv = request.httpv,
      .status_code = 500,
      .reason_phrase = "bad function"
    };
  }

  std::string tmp = response.dump();
  send(client, tmp.c_str(), tmp.size(), 0);
  close(client);
}
