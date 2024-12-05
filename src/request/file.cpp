#include "file.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <map>

namespace
{
  const file_ex match_ex(const std::filesystem::path & path)
  {
    const std::map< const std::filesystem::path, const file_ex > ex_map = {
      {".mp4", file_ex::MP4},
      {".avi", file_ex::AVI},
      {".av1", file_ex::AV1},
      {".mov", file_ex::MOV},
      {".m4v", file_ex::M4V},
      {"mpg", file_ex::MPG},
      {"wmv", file_ex::WMV}
    };
    try
    {
      return ex_map.at(path.extension());
    }
    catch (...)
    {
      throw std::runtime_error("bad extension: " + path.extension().string());
    }
  }
}

file_fd get_file(const std::string & path)
{
  std::filesystem::path fpath(path);

  std::ifstream in(fpath);
  if (!in.is_open())
  {
    throw std::runtime_error("error path: " + fpath.string());
  }
  in.close();

  file_fd file = {
    .path = fpath,
    .size = std::filesystem::file_size(fpath),
    .in_fmt = match_ex(fpath),
    .out_fmt = file_ex::NOTSET
  };
  return file;
}
