#ifndef FS1RGEN_MODEL
#define FS1RGEN_MODEL

#include <xercesc/dom/DOM.hpp>
#include <boost/any.hpp>
#include <vector>

using std::vector;

class Param {
public:
  virtual Param() = 0;
  virtual Param(const Param &) = 0;
  virtual ~Param() = 0;
  virtual bool validate(const boost::any & value) const = 0;
  virtual boost::any mutateWith(const vector<boost::any> & values) const = 0;
  virtual bool hasSubParams() = 0;
  virtual vector<Param*> subParams() = 0;
};

class DataModel {
public:
  DataModel(DOMNode *);
  DataModel(const DataModel &); // copy constructor
  const vector<DataModel> & getSubModels();
  const vector<Param*> & getBaseParams();
  const vector<Param*> & getNamedParams();
private:
  vector<DataModel> subModels_;
  vector<Param*> baseParams_;
  vector<Param*> cachedNamedParams_;
};

#endif
