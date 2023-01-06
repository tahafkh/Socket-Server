#include "../include/utilities.hpp"
#include "../include/include.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

map<string, string> get_extensions() {
  map<string, string> extensions;
  extensions["htm"] = "html";
  extensions["jpg"] = "jpeg";
  extensions["mp3"] = "mpeg";
  return extensions;
}

map<string, string> extensions = get_extensions();

string read_file(const char *filename) {
  ifstream infile;
  infile.open(filename, infile.binary);
  if (!infile.is_open()) return string();

  infile.seekg(0, infile.end);
  size_t length = infile.tellg();
  infile.seekg(0, infile.beg);

  if (length > BUFFER_SIZE) length = BUFFER_SIZE;
  char* buffer = new char[length + 1];

  infile.read(buffer, length);

  string s(buffer, length);
  delete[] buffer;
  return s;
}

string read_file(string filename) { return read_file(filename.c_str()); }

string get_extension(string file_path) {
  size_t pos = file_path.find_last_of('.');
  if (pos == string::npos) return string();
  string extension = file_path.substr(pos + 1);
  if (extensions.find(extension) != extensions.end()) return extensions[extension];
  return extension;
}

vector<string> split(string s, string delimiter, bool trim) {
  vector<string> tokens;
  if (trim) s.erase(remove(s.begin(), s.end(), ' '), s.end());
  size_t pos = 0;
  string token;
  while ((pos = s.find(delimiter)) != string::npos) {
    token = s.substr(0, pos);
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(s);
  return tokens;
}

vector<string> tokenize(string const &s, char delimiter) {
  vector<string> tokens;
  stringstream ss(s);
  string item;
  while (getline(ss, item, delimiter)) {
    tokens.push_back(item);
  }
  return tokens;
}

string encode_url(string const &str) {
  char encode_buf[4];
  string result;
  encode_buf[0] = '%';
  result.reserve(str.size());

  for (size_t pos = 0; pos < str.size(); ++pos) {
    switch (str[pos]) {
    default:
      if (str[pos] >= 32 && str[pos] < 127) {
        result += str[pos];
        break;
      }
    case '$':
    case '&':
    case '+':
    case ',':
    case '/':
    case ':':
    case ';':
    case '=':
    case '?':
    case '@':
    case '"':
    case '<':
    case '>':
    case '#':
    case '%':
    case '{':
    case '}':
    case '|':
    case '\\':
    case '^':
    case '~':
    case '[':
    case ']':
    case '`':
      sprintf(encode_buf + 1, "%02X", str[pos]);
      result += encode_buf;
      break;
    }
  };
  return result;
}

string decode_url(string const &str) {
  char decode_buf[3];
  string result;
  result.reserve(str.size());

  for (size_t pos = 0; pos < str.size(); ++pos) {
    switch (str[pos]) {
    case '+':
      result += ' ';
      break;
    case '%':
      if (pos + 2 < str.size()) {
        decode_buf[0] = str[++pos];
        decode_buf[1] = str[++pos];
        decode_buf[2] = '\0';
        result += static_cast<char>(strtol(decode_buf, nullptr, 16));
      } else {
        result += '%';
      }
      break;
    default:
      result += str[pos];
    }
  }
  return result;
}

char lowercase_helper(char in) {
  if (in <= 'Z' && in >= 'A')
    return in - ('Z' - 'z');
  return in;
}

string lowercase(string s) {
  transform(s.begin(), s.end(), s.begin(), lowercase_helper);
  return s;
}

bool comp::operator()(const string &lhs, const string &rhs) const {
  return lowercase(lhs) < lowercase(rhs);
}

csmap get_csmap_from_string(string str) {
  csmap m;
  vector<string> tokenized = tokenize(str, '&');
  for (auto token : tokenized) {
    vector<string> key_value = tokenize(token, '=');
    if (key_value.size() != 2)
      continue;
    string key = key_value[0];
    string value = key_value[1];
    m[key] = value;
  }
  return m;
}
