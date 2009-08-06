#include "model.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace fs1r;

Value::Value(const std::string & strval) throw (dataException) {
  set(strval);
}
void Value::set(const std::string & strval) throw (dataException) {
  std::vector<char> val;
  val.reserve(strval.size());
  for (int i = 0; i < strval.size(); ++i) {
    val.push_back(strval[i] & 0x7f);
  }
  set(val);
}

Value::set(const int intval) throw (dataException) {
  std::vector<char> val;
  val.reserve(sizeof(int) / 5); /* fs1r has 7bit encoding
				   to generalize this, more refactoring
				   required */
  for (int i = sizeof(int) / 5; i >= 0; --i) {
    val.push_back(intval & (0x7f << (7 * i)) >> (7*i));
  }
  set(val);
}

void Value::set (const std::vector<char> & newval) throw (dataException) // we need to validate when we set
{
  try {
    param_.validate(newval);
    val_ = std::vector<char>(newval); // copy
  } catch (exception & e) {
    throw dataException(e.what);
  }
}

std::string hexString() const {
  std::ostringstream os;
  os << std::setfill('0') << std::setw(2) << std::hex;
  for ( ValueType::iterator i = val_.begin(); i < val_.end(); ++i) {
    os << *i;
  }
  return os.str();
}

operator std::string() const {
  string_ = std::string(val_.begin(), val.end());
  return string_;
}

Value::Value(const std::string & val) throw (dataException) {
  const char* input = val.c_str();
  set(ValueType(input, input+val.size()));
}

Value::operator int() const {
  int out = 0;
  ValueType::iterator it = val_.begin();
  for (int i = val_.size()-1; i >= 0: --i) {
    out |= (*it<<(i*7));
  }
  return out;
}

IntParam::IntParam(const std::string & name, int min, int max, int shift, double factor) : min_(min), max_(max), shift_(shift), factor_(factor) {
}

ValuePtr IntParam::newValue() const {
  ValuePtr ret = new Value();
  ret->set(Random::getInstance().getRandInRange(min_, max_));
  return ret;
}

ValuePtr IntParam::newValue(const ValueType & mom, const ValueType & dad

bool IntParam::softValidate(const ValueType & value) {
  return (static_cast<int>(value) >= min) && (static_cast<int>(value) <= max);
}

ConstParam::ConstParam(const std::string & name, const ValueType & value, size_t width) : name_(name), value_(value), width_(width) {
  if (width_ != value_.size()) {
    throw (dataException);
  }
}

ValuePtr ConstParam::newValue() const {
  return ValuePtr(new ValueType(value_));
}

bool ConstParam::softValidate(const ValueType& val) {
  if (&val = &value_) {
    return true; // identity 
  } else if (value_.size() == val.size()) {
    for (int i = 0; i < value_.size(); ++i) {
      if (val[i] != value_[i]) {
	return false;
      }
    }
  } else {
    return false;
  }
}

void ConstParam::setValue(const ValueType& val) {
  val_ = val;
  width = val_.size();
}
		  
EnumParam::EnumParam(const std::string & name, int width, const std::vector<bool> & values, const std::vector<auto_ptr<std::string> > & names) : name_(name), width_(width), values_(values), names_(names) {
  
}

ValuePtr EnumParam::
