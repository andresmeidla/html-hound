#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP
#include <vector>
#include <map>
#include <string>

namespace XmlHound {

struct Position
{
  int start;
  int len;
  Position() {}
  Position(int start, int len) : start(start), len(len) {}
};

int GetElements(const std::string& xml, const std::string& tagStart, std::vector<Position>& tags);

}

#endif // XMLPARSER_HPP
