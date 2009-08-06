#ifndef FS1RGEN_MODEL
#define FS1RGEN_MODEL

#include <memory>
#include <exception>
#include <vector>
#include <set>
#include <string>
#include <boost/any.hpp>
#include "ga/ga.h"
#include <ctime>
#include <cstdlib>

using std::auto_ptr;

namespace fs1rgen {
  class FGenome;
  class Value;
  typedef auto_ptr<FGenome> FGenomePtr;
  typedef auto_ptr<Value> ValuePtr;
  typedef std::vector<char> ValueType;
  typedef TableKey char;

  class Random {
  private:
    Random() {}
    static Random instance_;
    static void init() {
      srand(time(NULL));
    }
  public:
    static Random & getInstance() { init(); return instance_; }
    int getRand() {
      return rand();
    }
    int getRandInRange(int min, int max) throws (exception){
      if (min >= max) { throw exception("Range bad"); }
      return rand() % (max - min) + min;
    }
  };

  class dataException : public exception {
    // dataException means there is a bug in the program
    // it means that the GA has attempted to set a value 
    // that does not fit the basic specs given in the xml file
  }

  class FGenome : public GAGenome { // contains params
  public:
    // see file:///C:/code/fs1rgen/src/galib247/doc/Extensions.html
    GADefineIdentity("fs1rgenGenome", 218);
    static void Init(GAGenome&);
    static int Mutate(GAGenome&, float);
    static float Compare(const GAGenome&, const GAGenome&);
    static float Evaluate(const GAGenome&, const GAGenome&, auto_ptr<GAGenome>,auto_ptr<GAGenome>);
    FGenome();
    FGenome(const FGenome& other);
    virtual ~FGenome();
    MyGenome& operator=(const GAGenome& other) {
      if (&orig != this) copy(orig);
      return *this;
    }
    virtual auto_ptr<GAGenome*> clone(CloneMethod) const { return new FGenome(*this); }
    virtual void copy(const GAGenome& orig) {
      GAGenome::copy(orig);
      // TODO copy values_
    }
  private:
    static std::list<Param> params_;
    static std::set<LinkParam*> links_;
    // static paramsLock_; TODO proper locks, although params_ should be read only once loaded
    std::list<Value> values_;
  };

  class Param {
  public:
    virtual ~Param() {}
    virtual ValuePtr newValue() const = 0; // new value across whole range of param
    virtual ValuePtr newValue(const ValueType &, float) const = 0; // new value based on single parent
    virtual ValuePtr newValue(const ValueType&, const ValueType&) const = 0; // new value based on two parents
    virtual void validate(const ValueType & val) throw (dataException) {
      if (!this->softValidate(val)) {
	throw dataException(std::string(typeid(*this).name()) + " failed validation of value " + val.hexString() );
      }
    }
    virtual bool softValidate(const ValueType & val) = 0;
    virtual size_t width() { return width_; }
    virtual void setWidth(size_t width) { width_ = width; }
  protected:
    std::string name_;
    size_t width_;
  };

  class IntParam : public Param {
  public:
    IntParam(const std::string & name, int min, int max, int shift = 0, double factor = 1.0);
    virtual ValuePtr newValue() const;
    virtual ValuePtr newValue(const ValueType&, float) const;
    virtual ValuePtr newValue(const ValueType&, const ValueType&) const;
    virtual bool softValidate(const ValueType & val);
    virtual void setMin(int min) { min_ = min; }
    virtual void setMax(int max) { max_ = max; }
    virtual void setShift(int shift) { shift_ = shift; }
    virtual int shift() { return shift_; }
    virtual void setFactor(double factor) { factor_ = factor; }
    virtual double factor() { return factor_; }
  private:
    int min_;
    int max_;
    int semanticShift_;
    double factor_;
  };

  class ConstParam : public Param {
  public:
    ConstParam(const std::string & name, const ValueType & value, size_t width);
    virtual ValuePtr newValue() const;
    virtual ValuePtr newValue(const ValueType&, float) const;
    virtual ValuePtr newValue(const ValueType&, const ValueType&) const;
    virtual bool softValidate(const ValueType & val);
    virtual void setValue(const ValueType & val);
  private:
    ValueType value_;
  };
  
  class EnumParam : public Param {
  public:
    EnumParam(const std::string & name, int width, const std::vector<bool> values, const std::vector<auto_ptr<std::string> > & names);
    virtual ValuePtr newValue() const;
    virtual ValuePtr newValue(const ValueType&, float) const;
    virtual ValuePtr newValue(const ValueType&, const ValueType&) const;
    virtual bool softValidate(const ValueType & val);
    virtual void setValue(size_t index, bool allowed);
    virtual void setName(size_t index, auto_ptr<std::string> & name);
  private:
    std::vector<bool> values_;
    std::vector<auto_ptr<std::string> > names_;
  };

  class BoolParam : public Param {
  public:
    BoolParam(const BoolParam &);
    BoolParam(const std::string & name);
    virtual ValuePtr newValue() const;
    virtual ValuePtr newValue(const ValueType&, float) const;
    virtual ValuePtr newValue(const ValueType&, const ValueType&) const;
    virtual bool softValidate(const ValueType & val);
  };

  class MultiParam : public Param {
  public:
    MultiParam(const MultiParam &);
    MultiParam(const std::string & name, const std::map<int, auto_ptr<Param> > & params);
    virtual ValuePtr newValue() const;
    virtual ValuePtr newValue(const ValueType&, float) const;
    virtual ValuePtr newValue(const ValueType&, const ValueType&) const;
    virtual bool softValidate(const ValueType & val);
    virtual void setParam(int index, auto_ptr<Param> param);
  private:
    std::map<int, auto_ptr<Param> > params_;
  };
  
  class LinkParam : public Param {
  public:
    LinkParam(const LinkParam &);
    LinkParam(const std:string & name, const std::set<TableKey> & tableRef) : name_(name),tableRef_(tableRef) {}
    virtual ValuePtr newValue() const;
    virtual ValuePtr newValue(const ValueType&, float) const;
    virtual ValuePtr newValue(const ValueType&, const ValueType&) const;
    virtual bool softValidate(const ValueType & val);
  private:
    const std::set<TableKey> & tableRef_;
  };

  class BitfieldParam : public Param {
  public:
    BitfieldParam(const BitfieldParam &);
    BitfieldParam(const std::string & name, size_t minBit, size_t maxBit);
    virtual ValuePtr newValue() const;
    virtual ValuePtr newValue(const ValueType&, float) const;
    virtual ValuePtr newValue(const ValueType&, const ValueType&) const;
    virtual bool softValidate(const ValueType & val);
    virtual void setBit(int index, bool used, const std::string & name);
  private:
    std::vector<bool> bitsUsed_;
    std::vector<auto_ptr<std::string> > names_;
    size_t minBit_;
    size_t maxBit_;
  };

  class OrParam : public Param {
  public:
    OrParam(const OrParam &);
    OrParam(const std::string & name);
    virtual void setParam(size_t shift, const Param & param);
    virtual bool softValidate(const ValueType & val);
    virtual ValuePtr newValue() const;
    virtual ValuePtr newValue(const ValueType&, float) const;
    virtual ValuePtr newValue(const ValueType&, const ValueType&) const;
  private:
    std::map<size_t,auto_ptr<Param> > params_;
  };

  class Value {
  public:
    Value(const Param & param, const ValueType & val) throw(dataException): param_(param),val_(val) {
      param_.validate();
    }
    Value(const Value & ref) throw (dataException): val_(ref.val_), param_(ref.param_) {}
    Value(const std::string & strval) throw (dataException);
    virtual operator int() const;
    virtual const operator std::string() const;
    virtual const ValueType & get() const { return val_; }
    virtual std::string hexString() const;
    virtual void set (const ValueType & newval) throw (dataException);
    virtual void set (const int newval) throw (dataException);
    virtual void set (const std::string & newval) throw (dataException);
    const Param & getParam() const { return param_; }
  private:
    ValueType val_;
    mutable std::string string_;
    Param param_;
  };
}

#endif
