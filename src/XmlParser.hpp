#ifndef FS1RGEN_XMLPARSE
#define FS1RGEN_XMLPARSE

#include <string>
#include <memory>

#include "params.h"

using namespace std;

namespace fs1rgen {
  class XmlParser {
  public:
    virtual void parse(const char *rawData, size_t bufSize) throw (exception) = 0;
    virtual void parse(const std::string & rawData) throw (exception) = 0;
    virtual auto_ptr<Model> getModel(); // constructs copy of model, caller owns
  private:
    Model m_model;
  };
}

#endif
