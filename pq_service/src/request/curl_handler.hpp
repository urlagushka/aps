#ifndef ASYNC_CURL_HANDLER_HPP
#define ASYNC_CURL_HANDLER_HPP

#include <string>
#include <vector>
#include <utility>
#include <format>
#include <future>
#include <thread>
#include <memory>
#include <iostream>
#include <curl/curl.h>

#include "json.hpp"

namespace
{
  static std::size_t on_write(char * ptr, std::size_t size, std::size_t nmemb, std::string * userdata)
  {
    if (userdata == nullptr)
    {
      return 0;
    }
    userdata->append(ptr, size * nmemb);
    return size * nmemb;
  }
}

template < typename answer_t >
concept correct_answer_t = requires(answer_t answer, const std::string & rhs)
{
  { answer = rhs } -> std::same_as< answer_t & >;
};

namespace curl
{
  using url_t = std::string;
  using query_t = std::vector< std::pair< std::string, std::string > >;

  class curl_handler
  {
    public:
      curl_handler(const std::string & user_agent);
      curl_handler(const curl_handler & rhs) = delete;
      curl_handler(curl_handler && rhs);
      curl_handler & operator=(const curl_handler & rhs) = delete;
      curl_handler & operator=(curl_handler && rhs);
      ~curl_handler();

      template < correct_answer_t answer_t >
      answer_t post(const url_t & url, const nlohmann::json & query);

      template < correct_answer_t answer_t >
      std::future< answer_t > async_post(const url_t & url, const nlohmann::json & query);

      template < correct_answer_t answer_t >
      answer_t get(const url_t & url);

      template < correct_answer_t answer_t >
      std::future< answer_t > async_get(const url_t & url);

    private:
      template < correct_answer_t answer_t >
      answer_t string_to_answer(const std::string & rhs);

      CURL * __curl;
      std::string __user_agent;
  };
}

template < correct_answer_t answer_t >
answer_t
curl::curl_handler::post(const url_t & url, const nlohmann::json & query)
{
  if (__curl == nullptr)
  {
    throw std::runtime_error("curl is null!");
  }

  std::string response;
  std::string tmp = query.dump();

  //curl_easy_setopt(__curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(__curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(__curl, CURLOPT_POST, 1L);
  curl_easy_setopt(__curl, CURLOPT_POSTFIELDS, tmp.c_str());
  curl_easy_setopt(__curl, CURLOPT_POSTFIELDSIZE, static_cast< long >(tmp.size()));

  curl_easy_setopt(__curl, CURLOPT_WRITEFUNCTION, on_write);
  curl_easy_setopt(__curl, CURLOPT_WRITEDATA, &response);

  curl_slist * headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, std::format("User-Agent: {}", __user_agent).c_str());
  curl_easy_setopt(__curl, CURLOPT_HTTPHEADER, headers);

  CURLcode code = curl_easy_perform(__curl);

  curl_slist_free_all(headers);
  if (code != CURLE_OK)
  {
    throw std::runtime_error(std::format("POST {}\n{}", url, curl_easy_strerror(code)));
  }

  return string_to_answer< answer_t >(response);
}

template < correct_answer_t answer_t >
std::future< answer_t >
curl::curl_handler::async_post(const url_t & url, const nlohmann::json & query)
{
  std::promise< answer_t > promise;
  std::future< answer_t > future = promise.get_future();

  std::thread([promise = std::move(promise), this, url, query]() mutable {
    try
    {
      promise.set_value(post< answer_t >(url, query));
    }
    catch (const std::runtime_error & error)
    {
      promise.set_exception(std::make_exception_ptr(error));
    }
  }).detach();

  return future;
}

template < correct_answer_t answer_t >
answer_t
curl::curl_handler::get(const url_t & url)
{
  if (__curl == nullptr)
  {
    throw std::runtime_error("curl is null!");
  }

  std::string response;

  curl_easy_setopt(__curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(__curl, CURLOPT_HTTPGET, 1L);

  curl_easy_setopt(__curl, CURLOPT_WRITEFUNCTION, on_write);
  curl_easy_setopt(__curl, CURLOPT_WRITEDATA, &response);

  curl_slist * headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
  headers = curl_slist_append(headers, std::format("User-Agent: {}", __user_agent).c_str());
  curl_easy_setopt(__curl, CURLOPT_HTTPHEADER, headers);

  CURLcode code = curl_easy_perform(__curl);

  curl_slist_free_all(headers);
  if (code != CURLE_OK)
  {
    throw std::runtime_error(std::format("GET {}\n{}", url, curl_easy_strerror(code)));
  }

  return string_to_answer< answer_t >(response);
}

template < correct_answer_t answer_t >
std::future< answer_t >
curl::curl_handler::async_get(const url_t & url)
{
  std::promise< answer_t > promise;
  std::future< answer_t > future = promise.get_future();

  std::thread([promise = std::move(promise), this, url]() mutable {
    try
    {
      promise.set_value(get< answer_t >(url));
    }
    catch (const std::runtime_error & error)
    {
      promise.set_exception(std::make_exception_ptr(error));
    }
  }).detach();

  return future;
}

template < correct_answer_t answer_t >
answer_t
curl::curl_handler::string_to_answer(const std::string & rhs)
{
  answer_t answer = nlohmann::json::parse(rhs);
  return answer;
}

#endif
