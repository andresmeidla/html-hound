#include <stdexcept>
#include <stack>
#include <stdio.h>
#include <string.h>
#include "xmlhound.h"

namespace
{
  const std::string START[] = {"<div", "<p", "<span", "<form", "<li", "<td", "<ul", "<section", "<table", "<article"};
  const std::string END[] = {"</div>", "</p>", "</span>", "</form>", "</li>", "</td>", "</ul>", "</section>", "</table>", "</article>"};
}
namespace XmlHound {

int FindTags(const std::string& xml, std::map<std::string, std::vector<Position> >& tags) {
  size_t index = 0;
  size_t xmlLen = xml.length();
  while((index = xml.find("<", index)) != std::string::npos) {
    //printf("%s\n", xml.substr(index, 30).c_str());
    for(std::map<std::string, std::vector<Position> >::iterator itr = tags.begin(); itr != tags.end(); ++itr) {
      const std::string& str = itr->first;
      if(index + str.length() <= xmlLen) {
        //printf("searching %s, from %s\n", str.c_str(), xml.substr(index, str.length()).c_str());
        if(memcmp(xml.c_str() + index, str.c_str(), str.length()*sizeof(char)) == 0) {
          //printf("found %s at %u\n", str.c_str(), index);
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
int GetElements(const std::string& xml, const std::string& tagStartWithAttrs, std::vector<Position>& tags)
{
  size_t tagEndLen;
	size_t tmp;

  if(tagStartWithAttrs.length() == 0 || tagStartWithAttrs[0] != '<') {
    fprintf(stderr, "invalid tagStartWithAttrs supplied: must start with <\n");
    return 1;
  }

  std::string tagEnd, tagStart;

  if((tmp = tagStartWithAttrs.find(' ')) != std::string::npos) {
    tagStart = tagStartWithAttrs.substr(0, tmp);
    tagEnd = tagStart + ">";
    tagEnd.insert(1, "/");
  }
  else {
    tagStart = tagStartWithAttrs;
    tagEnd = tagStartWithAttrs  + ">";
    tagEnd.insert(1, "/");
  }

  tagEndLen = tagEnd.length();

  std::map<std::string, std::vector<Position> > tagLocations;
  tagLocations[tagStartWithAttrs] = std::vector<Position>();
  tagLocations[tagStart] = std::vector<Position>();
  tagLocations[tagEnd] = std::vector<Position>();
  // search all occurrences of tagStartWithAttrs, tagStart and tagEnd
  FindTags(xml, tagLocations);
  std::vector<Position>::iterator endItr = tagLocations[tagEnd].begin();
  std::vector<Position>::iterator startItr = tagLocations[tagStart].begin();
  if(startItr == tagLocations[tagStart].end())
    return 0;

  std::stack<Position> stack;
  std::map<int, int> startTagMap;
  for(std::vector<Position>::iterator itr = tagLocations[tagStartWithAttrs].begin(); itr != tagLocations[tagStartWithAttrs].end(); ++itr) {
    startTagMap[itr->start] = itr->start;
  }
  stack.push(*startItr);
  ++startItr;

  // match start tags with end tags
  while(stack.size() > 0 && endItr != tagLocations[tagEnd].end()) {

		// check if the end tag closes the current tag in the stack
    if(endItr->start > stack.top().start &&
      (startItr == tagLocations[tagStart].end() || endItr->start < startItr->start)) {
      Position pos = stack.top();
      pos.len = endItr->start + tagEndLen - pos.start;
      ++endItr;
      stack.pop();
      if(startTagMap.find(pos.start) != startTagMap.end()) {
				// matches tag with attributes
        tags.push_back(pos);
      }

			// only add new start tag to the stack if there are none in the stack.
			// we want to close the tags in the stack next
      if(stack.size() == 0 && startItr != tagLocations[tagStart].end()) {
        stack.push(*startItr);
        ++startItr;
      }
    } else {
			// the end tag did not match the start tag, must search for an embedded element.
			// add a start tag
      if(startItr != tagLocations[tagStart].end()) {
        if(startItr != tagLocations[tagStart].end()) {
          stack.push(*startItr);
          ++startItr;
        }
      }
    }
  }

  return 0;
}

} // namespace
