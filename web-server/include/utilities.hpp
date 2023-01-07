#ifndef __UTILILITES_HPP__
#define __UTILILITES_HPP__

#include <cstring>
#include <map>
#include <string>
#include <vector>

struct comp {
  bool operator()(const std::string &lhs, const std::string &rhs) const;
};

typedef std::map<std::string, std::string, comp> csmap;

std::string read_file(const char *file_name);
std::string read_file(std::string file_name);

std::string get_extension(std::string file_path);

std::vector<std::string> split(std::string s, std::string d, bool trim=true);
std::vector<std::string> tokenize(std::string const &, char delimiter);

std::string encode_url(std::string const &);
std::string decode_url(std::string const &);

std::string lowercase(std::string);

csmap get_csmap_from_string(std::string);

#endif
