#include <stdexcept>
#include <stack>
#include <stdio.h>
#include <string.h>
#include <locale>
#include <codecvt>
#include "xmlhound.h"

namespace XmlHound {

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

int FindTags(const std::u32string& xml, std::map<std::u32string, std::vector<Position> >& tags) {
  size_t index = 0;
  size_t xmlLen = xml.length();
  while((index = xml.find(U"<", index)) != std::u32string::npos) {
    for(std::map<std::u32string, std::vector<Position> >::iterator itr = tags.begin(); itr != tags.end(); ++itr) {
      const std::u32string& str = itr->first;
      if(index + str.length() <= xmlLen) {
        if(memcmp(xml.c_str() + index, str.c_str(), str.length()*sizeof(str[0])) == 0) {
          itr->second.push_back(Position(index, 0));
        }
      }
    }
    ++index;
  }
  return 0;
}

/*
  Finds all the tags which start with the supllied tag from the supplied xml.
*/
bool GetElements(const std::u32string& xml, const std::u32string& tagStartWithAttrs, std::vector<Position>& positions)
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
  FindTags(xml, tagLocations);
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
bool GetElement(const std::u32string& xml, const std::u32string& tagStartWithAttrs, Position& pos) {
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
  size_t xmlLen = xml.length();

  // search for our tagStartWithAttrs
  if((index = xml.find(tagStartWithAttrs)) != std::u32string::npos) {
    pos.start = index;
    index += tagStartWithAttrs.length();
    while((index = xml.find(U"<", index)) != std::u32string::npos) {
      // check if the tag is a start tag
      if(index + tagStartLen <= xmlLen) {
        if(memcmp(xml.c_str() + index, tagItems.tagStart.c_str(), tagStartLen*sizeof(tagItems.tagStart[0])) == 0) {
          // an embedded start tag pushed to the stack
          stack.push(index);
        }
      }
      // check if the tag is an end tag
      if(index + tagEndLen <= xmlLen) {
        if(memcmp(xml.c_str() + index, tagItems.tagEnd.c_str(), tagEndLen*sizeof(tagItems.tagEnd[0])) == 0) {
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

} // namespace
