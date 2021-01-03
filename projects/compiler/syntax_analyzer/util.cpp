#include "./util.hpp"

#include <sstream>

std::string StartTag(const std::string& value) {
  std::stringstream tag;
  tag << "<" << value << ">";
  return tag.str();
}

std::string EndTag(const std::string& value) {
  std::stringstream tag;
  tag << "</" << value << ">";
  return tag.str();
}
