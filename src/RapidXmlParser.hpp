#ifndef FS1RGEN_RAPIDXMLPARSE
#define FS1RGEN_RAPIDXMLPARSE

#include "XmlParser.hpp"
#include "rapidxml.hpp"

using namespace rapidxml;
using namespace std;

namespace fs1rgen {
  class RapidXmlParser {
  public:
    RapidXmlParser();
    virtual ~RapidXmlParser();
    virtual void parse(const char *rawData, size_t bufSize) throw (exception);
    virtual void parse(const std::string & rawData) throw (exception);
  private:
    xml_document<> m_doc;
  };
}

#endif
