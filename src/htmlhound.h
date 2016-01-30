#ifndef HTMLPARSER_HPP
#define HTMLPARSER_HPP
#include <vector>
#include <map>
#include <string>

namespace HtmlHound {

struct Position
{
  int start;
  int len;
  Position() {}
  Position(int start, int len) : start(start), len(len) {}
};

bool GetElements(const std::u32string& html, const std::u32string& tagStart, std::vector<Position>& positions);
bool GetElement(const std::u32string& html, const std::u32string& tagStart, Position& pos);

}

#endif // HTMLPARSER_HPP
