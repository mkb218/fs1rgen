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

ValuePtr IntParam::newValue(const Value & mom, const Value & dad) {
  ValuePtr ret(new Value(*this, (static_cast<int>(mom)+static_cast<int>(dad)/2)));
  return ret;
}
    
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

ValuePtr ConstParam::newValue(const Value&, const Value&) {
  return newValue();
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
  ;
}
ValuePtr EnumParam::newValue() {
  std::vector<size_t> validValues;
  for (int i = 0; i < values_.size(); ++i) {
    if (values[i]) {
      validValues.push_back(i);
    }
  }

  return ValuePtr(new Value(*this, validValues[Random::getInstance.getRandInRange(0, validValues.size()-1)]));
}

ValuePtr EnumParam::newValue(const Value&mom, const Value&dad) {
  std::vector<size_t> validValues;
  
  for (int i = 0; i < values_.size(); ++i) {
    if (values[i]) {
      validValues.push_back(i);
      for(int j = 5 * ((static_cast<int>(dad) == i)+(static_cast<int>(mom) == i)); j > 0; --j) {
	validValues.push_back(i);
      }
    }
  }
  
  return ValuePtr(new Value(*this, validValues[Random::getInstance.getRandInRange(0, validValues.size()-1)]));
}

ValuePtr BoolParam::newValue() {
  return ValuePtr(new Value(*this, (Random::getInstance.getRandInRange(0,1))));
}

ValuePtr BoolParam::newValue(const Value& mom, const Value& dad) {
  bool newval;
  if (static_cast<int>(mom) && static_cast<int>(dad)) {
    newval = true;
  } else if (!static_cast<int>(mom) && !static_cast<int>(dad)) {
    newval = false;
  } else {
    newval = Random::getInstance.getRandInRange(0,1);
  }

  return ValuePtr(new Value(*this, newval));
}

bool BoolParam::softValidate(const ValueType& val) {
  return (val.size() == 1 && (val[0] == 0 || val[0] == 1));
}

// MultiParam newValue, what to do?!
// newValue(void) choose random param, then choose random value
ValuePtr MultiParam::newValue() const {
  int index = Random::getInstance().getRandInRange(0, params_.values()-1);
  return rangeMap.valueAtIndex(index).newValue();
}

// newValue(const Value&, const Value&) if both same param, use param.newvalue(const Value&, const Value&) else use random choice of one param and param.newValue()
ValuePtr multiParam::newValue(const Value & mom, const Value & dad) const {
  Param & momParam = params_.valueForPosition(static_cast<int>(mom));
  Param & dadParam = params_.valueForPosition(static_cast<int>(dad));
  if (momParam == dadParam || Random::getInstance.getRandInRange(0,1)) {
    return momParam.newValue(mom, dad);
  } else {
    return dadParam.newValue(mom, dad);
  }
}x

// LinkParam is special, no newvalue() is valid

// BitfieldParam
ValuePtr BitfieldParam::newValue() {
  int newVal = 0;
  for (int f = 0; f < bitsUsed_.size(); ++f) {
    newVal |= (Random::getInstance.getRandInRange(0,1) << f);
  }
  return ValuePtr(new Value(newVal));
}

/*
  mom dad  0   1
  0   0    75% 25%
  0   1    50  50
  1   0    50  50
  1   1    25  75
*/

ValuePtr BitfieldParam::newValue(const Value & mom, const Value & dad) {
  int newVal = 0;
  int denom, shift;
  bool boolmom = static_cast<int>(mom), booldad = static_cast<int>(dad);
  if (boolmom xor booldad) {
    shift = -4;
  } else {
    if (booldad) {
      shift = -2;
    } else {
      shift = 2;
    }
  }
    
  for (int f = 0; f < bitsUsed_.size(); ++f) {
    int result = (Random::getInstance.getRandInRange(0,7) + shift) >= 0;
    newVal |= (result << f);
  }
  return ValuePtr(new Value(newVal));
}


bool BitfieldParam::softValidate(const ValueType & val) {
  size_t count = 0;
  
  for(size_t i = val.size() - 1; i >= 0; --i) {
    for (size_t j = 6; j >= 0; --j) {
      // byte 1  0          
      // bit  9876543210
      // i    1  0
      // j    2106543210
      
      if (((i*7+j > maxBit_) || !bitsUsed_[i*7+j] || (i*7+j < minBit_)) && 
	  (val[val.size()-1-i] & (0x40 >> j))) {
	return false;
      }	  
    }
  } 

  return true;
}



// OrParam
// newValue no args - call newValue() for each subparam, shift and | together
ValuePtr OrParam::newValue() {
  int value = 0;
  std::map<size_t, auto_ptr<Param> >::iterator it;
  for (it = params_.begin(); it < params_.end(); ++it) {
    value |= (it->second->newValue() << it->first);
  }
  return value;
}

// newValue args - call newValue(mom,dad) for each subparam, shift and | together
ValuePtr OrParam::newValue(const Value & mom, const Value & dad) {
  int value = 0;
  std::map<size_t, auto_ptr<Param> >::iterator it;
  std::map<size_t, int> shiftToMask;
  std::vector<size_t> shifts;
  for (it = params_.begin(); it < params_.end(); ++it) {
    shifts.push_back(it->first);
  }
  std::sort(shifts);
  for (int i = 0; i+1 < shifts.size(); ++i) {
    // say shifts are 0,3,7
    // we need two masks: 0x07 and 0x0f
    int diff = shifts[i+1] - shifts[i];
    int mask = 0;
    for (int j = 0; j < diff; ++j) {
      // 0 1 2
      mask = (mask << 1) | 1;
    }
    shiftToMask.insert(shifts[i], mask);
  }

  for (it = params_.begin(); it < params_.end(); ++it) {
    int mask = 0xffffffff;
    if (shiftToMask.count(it->first) == 1) {
      mask = shiftToMask[it->first];
    }
    value |= (it->second->newValue((static_cast<int>(mom) >> it->first) & mask,(static_cast<int>(mom) >> it->first) & mask) << it->first);
  }
  
  return value;  
}

// softValidate - call softvalidate on each subparam with pre-shifted value
bool OrParam::softValidate(const ValueType & val) {
  
}
