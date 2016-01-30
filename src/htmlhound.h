#ifndef HTMLPARSER_HPP
#define HTMLPARSER_HPP
#include <vector>
#include <map>
#include <string>

namespace HtmlHound {

struct Position {
  int start;
  int len;
  Position(): start(-1), len(-1) {}
  Position(int start, int len) : start(start), len(len) {}
};

struct Link {
  Position pos;
  std::u32string url;
  std::u32string text;
  Link() {}
  Link(const std::u32string& url, const std::u32string& text): url(url), text(text) {}
};

bool getElements(const std::u32string& html, const std::u32string& tagStart, std::vector<Position>& positions);
bool getElement(const std::u32string& html, const std::u32string& tagStart, Position& pos);
bool parseLink(const std::u32string& html, Link& link);

}

#endif // HTMLPARSER_HPP
