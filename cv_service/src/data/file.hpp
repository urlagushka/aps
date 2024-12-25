#ifndef FILE_HPP
#define FILE_HPP

#include <cstddef>
#include <filesystem>
#include <string>

enum class file_ex
{
  MP4,
  AVI,
  AV1,
  MOV,
  M4V,
  MPG,
  WMV,
  NOTSET
};

struct file_fd
{
  std::filesystem::path path;
  std::size_t size;
  file_ex in_fmt;
  file_ex out_fmt;
};

file_fd get_file(const std::string & path);

#endif
