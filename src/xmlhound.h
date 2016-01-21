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

bool GetElements(const std::u32string& xml, const std::u32string& tagStart, std::vector<Position>& positions);
bool GetElement(const std::u32string& xml, const std::u32string& tagStart, Position& pos);

}

#endif // XMLPARSER_HPP
