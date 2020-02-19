#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#include <string>
#include <vector>
#include <set>
#include <istream>

static std::set<char> delims{'/'};

static std::string file_name_from_path(const std::string& str, const std::set<char> delimiters = delims)
{
  std::vector<std::string> result;

  char const* pch = str.c_str();
  char const* start = pch;
  for(; *pch; ++pch)
  {
    if (delimiters.find(*pch) != delimiters.end())
    {
      if (start != pch)
      {
        std::string str(start, pch);
        result.push_back(str);
      }
      else
      {
        result.push_back("");
      }
      start = pch + 1;
    }
  }
  result.push_back(start);

  return result.back();
}

static size_t filesize(const std::string & filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    size_t res = in.tellg();
    in.close();
    return res;
}

static size_t filesize(std::istream & file)
{
    int curr_pos = file.tellg();
    file.seekg(0, ios_base::end);
    size_t res = file.tellg();
    file.seekg(curr_pos, ios_base::beg);
    return res;
}

#endif // PATH_UTILS_H