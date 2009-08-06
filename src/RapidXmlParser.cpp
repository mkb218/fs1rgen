#include "RapidXmlParser.hpp"

#include <string>
#include <memory>
using namespace fs1rgen;
using namespace rapidxml;

void RapidXmlParser::parse(const char *xmlData, size_t bufSize) throw (exception) {
  // TODO: validate that a NUL exists at xmlData[bufSize-1] or before if rapidXml dies with such input
  // only to avoid copying file to new buffer which misses the point of mmap()ing it
  // rapidxml should specialize on parse_non_destructive so it doesn't try to write to arg if it has non destrucitve tag. otherwise, why make it a template arg instead of a method arg?
  m_doc.parse<parse_non_destructive>(const_cast<char*>(xmlData));
  
}

void RapidXmlParser::parse(const std::string & rawData) throw (exception) {
  parse(rawData.c_str(), rawData.size()+1);
}
