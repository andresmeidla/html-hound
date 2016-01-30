#include <stdexcept>
#include <stack>
#include <stdio.h>
#include <string.h>
#include <locale>
#include <codecvt>
#include "htmlhound.h"

namespace HtmlHound {

struct TagItems {
  std::u32string tagStartWithAttrs;
  std::u32string tagStart;
  std::u32string tagEnd;
  TagItems(const std::u32string& tagStartWithAttrs) {
    size_t tmp;
    if(tagStartWithAttrs.length() > 0 && (tmp = tagStartWithAttrs.find(' ')) != std::u32string::npos) {
      tagStart = tagStartWithAttrs.substr(0, tmp);
      tagEnd = tagStart + U">";
      tagEnd.insert(1, U"/");
    }
    else {
      tagStart = tagStartWithAttrs;
      tagEnd = tagStartWithAttrs  + U">";
      tagEnd.insert(1, U"/");
    }
  }
};

int findTags(const std::u32string& html, std::map<std::u32string, std::vector<Position> >& tags) {
  size_t index = 0;
  size_t htmlLen = html.length();
  while((index = html.find(U"<", index)) != std::u32string::npos) {
    for(std::map<std::u32string, std::vector<Position> >::iterator itr = tags.begin(); itr != tags.end(); ++itr) {
      const std::u32string& str = itr->first;
      if(index + str.length() <= htmlLen) {
        if(memcmp(html.c_str() + index, str.c_str(), str.length()*sizeof(str[0])) == 0) {
          itr->second.push_back(Position(index, 0));
        }
      }
    }
    ++index;
  }
  return 0;
}

/*
  Finds all the tags which start with the supllied tag from the supplied html.
*/
bool getElements(const std::u32string& html, const std::u32string& tagStartWithAttrs, std::vector<Position>& positions)
{
  size_t tagEndLen;

  if(tagStartWithAttrs.length() == 0 || tagStartWithAttrs[0] != U'<') {
    fprintf(stderr, "invalid tagStartWithAttrs supplied: must start with <\n");
    return false;
  }
  TagItems tagItems(tagStartWithAttrs);
  tagEndLen = tagItems.tagEnd.length();

  std::map<std::u32string, std::vector<Position> > tagLocations;
  tagLocations[tagStartWithAttrs] = std::vector<Position>();
  tagLocations[tagItems.tagStart] = std::vector<Position>();
  tagLocations[tagItems.tagEnd] = std::vector<Position>();
  // search all occurrences of tagStartWithAttrs, tagStart and tagEnd
  findTags(html, tagLocations);
  std::vector<Position>::iterator endItr = tagLocations[tagItems.tagEnd].begin();
  std::vector<Position>::iterator startItr = tagLocations[tagItems.tagStart].begin();
  if(startItr == tagLocations[tagItems.tagStart].end())
    return true;

  std::stack<Position> stack;
  std::map<int, int> startTagMap;
  for(std::vector<Position>::iterator itr = tagLocations[tagStartWithAttrs].begin(); itr != tagLocations[tagStartWithAttrs].end(); ++itr) {
    startTagMap[itr->start] = itr->start;
  }
  stack.push(*startItr);
  ++startItr;

  // match start tags with end tags
  while(stack.size() > 0 && endItr != tagLocations[tagItems.tagEnd].end()) {
    // check if the end tag closes the current tag in the stack
    if(endItr->start > stack.top().start &&
      (startItr == tagLocations[tagItems.tagStart].end() || endItr->start < startItr->start)) {
      Position pos = stack.top();
      pos.len = endItr->start + tagEndLen - pos.start;
      ++endItr;
      stack.pop();
      if(startTagMap.find(pos.start) != startTagMap.end()) {
        // matches tag with attributes
        positions.push_back(pos);
      }

      // only add new start tag to the stack if there are none in the stack.
      // we want to close the tags in the stack next
      if(stack.size() == 0 && startItr != tagLocations[tagItems.tagStart].end()) {
        stack.push(*startItr);
        ++startItr;
      }
    } else {
      // the end tag did not match the start tag, must search for an embedded element.
      // add a start tag
      if(startItr != tagLocations[tagItems.tagStart].end()) {
        if(startItr != tagLocations[tagItems.tagStart].end()) {
          stack.push(*startItr);
          ++startItr;
        }
      }
    }
  }

  return true;
}

/*
 * Find the first
 */
bool getElement(const std::u32string& html, const std::u32string& tagStartWithAttrs, Position& pos) {
  size_t index;
  pos.start = -1;
  pos.len = -1;

  if(tagStartWithAttrs.length() == 0 || tagStartWithAttrs[0] != U'<') {
    fprintf(stderr, "invalid tagStartWithAttrs supplied: must start with <\n");
    return false;
  }
  TagItems tagItems(tagStartWithAttrs);
  std::stack<int> stack;
  size_t tagEndLen = tagItems.tagEnd.length();
  size_t tagStartLen = tagItems.tagStart.length();
  size_t htmlLen = html.length();

  // search for our tagStartWithAttrs
  if((index = html.find(tagStartWithAttrs)) != std::u32string::npos) {
    pos.start = index;
    index += tagStartWithAttrs.length();
    while((index = html.find(U"<", index)) != std::u32string::npos) {
      // check if the tag is a start tag
      if(index + tagStartLen <= htmlLen) {
        if(memcmp(html.c_str() + index, tagItems.tagStart.c_str(), tagStartLen*sizeof(tagItems.tagStart[0])) == 0) {
          // an embedded start tag pushed to the stack
          stack.push(index);
        }
      }
      // check if the tag is an end tag
      if(index + tagEndLen <= htmlLen) {
        if(memcmp(html.c_str() + index, tagItems.tagEnd.c_str(), tagEndLen*sizeof(tagItems.tagEnd[0])) == 0) {
          if(stack.size()) {
            // close tag for the last embedded start tag
            stack.pop();
          }
          else {
            // we have found the end tag to our initial tagStartWithAttrs
            pos.len = index - pos.start + tagEndLen;
            break;
          }
        }
      }
      ++index;
    }
  }

  return true;
}

bool parseLink(const std::u32string& html, Link& link) {
  size_t index, tmp;
  bool ret = false;
  Position pos;
  static const std::u32string LINK_START = std::u32string(U"<a");
  static const std::u32string LINK_END = std::u32string(U"</a>");
  static const std::u32string LINK_HREF = std::u32string(U"href=");
  static const char32_t GT = U'>';
  char32_t quoteChar;

  // find <a
  if((index = html.find(LINK_START)) != std::u32string::npos) {
    pos.start = index;
    index += LINK_START.length();
    // find href=
    if((index = html.find(LINK_HREF, index)) != std::u32string::npos) {
      index += LINK_HREF.length();
      // advance index by ine (quote char)
      quoteChar = html[index];
      ++index;
      tmp = index;
      // find the end of the quotation
      if((index = html.find(quoteChar, index)) != std::u32string::npos) {
        link.url = html.substr(tmp, index - tmp);
        // find >
        if((index = html.find(GT, index)) != std::u32string::npos) {
          ++index;
          tmp = index;
          // find </a>
          if((index = html.find(LINK_END, index)) != std::u32string::npos) {
            pos.len = index + LINK_END.length() - pos.start;
            link.text = html.substr(tmp, index - tmp);
            link.pos = pos;
            ret = true;
          }
        }
      }
    }
  }

  return ret;
}

} // namespace
