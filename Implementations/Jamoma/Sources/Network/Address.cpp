#include "Network/JamomaAddress.h"

# pragma mark -
# pragma mark Life cycle

JamomaAddress::JamomaAddress(shared_ptr<Device> device, TTObject aData) :
mDevice(device),
mData(aData),
mValueType(Value::Type::IMPULSE),
mAccessMode(AccessMode::BI),
mBoundingMode(BoundingMode::FREE),
mRepetitionFilter(false)
{
  // edit value type, access mode, bounding mode and repetition filter attribute
  if (mData.valid())
  {
    TTSymbol objectName = mData.name();
    
    if (objectName == kTTSym_Mirror)
      objectName = TTMirrorPtr(mData.instance())->getName();
    
    if (objectName == "Data")
    {
      TTSymbol type;
      mData.get("type", type);
      
      if (type == kTTSym_none)
      {
        mValue = new Impulse();
        mValueType = Value::Type::IMPULSE;
      }
      else if (type == kTTSym_generic)
      {
        mValue = new OSSIA::Tuple();
        mValueType = Value::Type::TUPLE;
      }
      else if (type == kTTSym_boolean)
      {
        mValue = new OSSIA::Bool();
        mValueType = Value::Type::BOOL;
      }
      else if (type == kTTSym_integer)
      {
        mValue = new OSSIA::Int();
        mValueType = Value::Type::INT;
      }
      else if (type == kTTSym_decimal)
      {
        mValue = new OSSIA::Float();
        mValueType = Value::Type::FLOAT;
      }
      else if (type == kTTSym_array)
      {
        mValue = new OSSIA::Tuple();
        mValueType = Value::Type::TUPLE;
      }
      else if (type == kTTSym_string)
      {
        mValue = new OSSIA::String();
        mValueType = Value::Type::STRING;
      }
      
      TTSymbol service;
      mData.get("service", service);
      
      if (service == kTTSym_parameter)
        mAccessMode = AccessMode::BI;
      else if (service == kTTSym_message)
        mAccessMode = AccessMode::SET;
      else if (service == kTTSym_return)
        mAccessMode = AccessMode::GET;
      
      TTValue range;
      mData.get("rangeBounds", range);
      
      if (type == kTTSym_none)
      {
        mDomain = Domain::create();
      }
      else if (type == kTTSym_generic)
      {
        mDomain = Domain::create();
      }
      else if (type == kTTSym_boolean)
      {
        if (range.size() == 2)
        {
          Value * min = new OSSIA::Bool(range[0]);
          Value * max = new OSSIA::Bool(range[1]);
          mDomain = Domain::create(min, max);
        }
      }
      else if (type == kTTSym_integer)
      {
        if (range.size() == 2)
        {
          Value * min = new OSSIA::Int(range[0]);
          Value * max = new OSSIA::Int(range[1]);
          mDomain = Domain::create(min, max);
        }
      }
      else if (type == kTTSym_decimal)
      {
        if (range.size() == 2)
        {
          Value * min = new OSSIA::Float(range[0]);
          Value * max = new OSSIA::Float(range[1]);
          mDomain = Domain::create(min, max);
        }
      }
      else if (type == kTTSym_array)
      {
        // we need to know the size of the array to setup the domain
        TTValue v;
        mData.get("value", v);
        
        vector<const Value*> tuple_min;
        vector<const Value*> tuple_max;
        for (unsigned long i = 0; i < v.size(); i++)
          tuple_min.push_back(new OSSIA::Float(range[0]));
        tuple_max.push_back(new OSSIA::Float(range[1]));
        
        mDomain = Domain::create(new OSSIA::Tuple(tuple_min), new OSSIA::Tuple(tuple_max));
      }
      else if (type == kTTSym_string)
      {
        // string values enumeration
        vector<const Value*> values;
        for (const auto & e : range)
        {
          TTSymbol s = e;
          values.push_back(new OSSIA::String(s.c_str()));
        }
        mDomain = Domain::create(new OSSIA::Impulse(), new OSSIA::Impulse(), values);
      }
      else
      {
        mDomain = Domain::create();
      }
      
      TTSymbol clipmode;
      mData.get("rangeClipmode", clipmode);
      
      if (clipmode == kTTSym_none)
        mBoundingMode = BoundingMode::FREE;
      else if (clipmode == kTTSym_low)
        mBoundingMode = BoundingMode::CLIP;
      else if (clipmode == kTTSym_high)
        mBoundingMode = BoundingMode::CLIP;
      else if (clipmode == kTTSym_both)
        mBoundingMode = BoundingMode::CLIP;
      else if (clipmode == kTTSym_wrap)
        mBoundingMode = BoundingMode::WRAP;
      else if (clipmode == kTTSym_fold)
        mBoundingMode = BoundingMode::FOLD;
      
      mData.get("repetitionFilter", mRepetitionFilter);
      
      // enable callback to be notified each time the value change
      TTObject    callback("callback");
      TTValue     args(TTPtr(this), mData);
      callback.set("baton", args);
      callback.set("function", TTPtr(&JamomaAddress::ValueCallback));
      
      TTAttributePtr attribute;
      mData.instance()->findAttribute("value", &attribute);
      attribute->registerObserverForNotifications(callback);
    }
  }
}

JamomaAddress::~JamomaAddress()
{}

# pragma mark -
# pragma mark Value

bool JamomaAddress::updateValue() const
{
  TTValue v;
  mData.get("value", v);
  
  // clear former value
  delete mValue;
  
  // create new value
  mValue = convertTTValueIntoValue(v, mValueType);
  
  return mValue != nullptr;
}

const Value * JamomaAddress::getValue() const
{
  return mValue;
}

bool JamomaAddress::sendValue(const Value * value) const
{
  // clear former value
  delete mValue;
  
  // copy the new value
  mValue = value->clone();
  
  TTValue v;
  convertValueIntoTTValue(mValue, v);
  
  if (mData.name() == "Data")
    return !mData.send("Command", v);
  else
    return !mData.set("value", v);
}

# pragma mark -
# pragma mark Network

const shared_ptr<Device> & JamomaAddress::getDevice() const
{
  return mDevice;
}

# pragma mark -
# pragma mark Accessors

Value::Type JamomaAddress::getValueType() const
{
  return mValueType;
}

JamomaAddress::AccessMode JamomaAddress::getAccessMode() const
{
  return mAccessMode;
}

Address & JamomaAddress::setAccessMode(AccessMode accessMode)
{
  mAccessMode = accessMode;
  
  if (mAccessMode == AccessMode::BI)
    mData.set("service", kTTSym_parameter);
  else if (mAccessMode == AccessMode::SET)
    mData.set("service", kTTSym_message);
  else if (mAccessMode == AccessMode::GET)
    mData.set("service", kTTSym_return);
  
  return *this;
}

const shared_ptr<Domain> & JamomaAddress::getDomain() const
{
  return mDomain;
}

Address & JamomaAddress::setDomain(shared_ptr<Domain> domain)
{
  mDomain = domain;
  
  TTValue range, v;
  
  if (mDomain->getValues().empty())
  {
    convertValueIntoTTValue(mDomain->getMin(), v);
    range.append(v);
    
    convertValueIntoTTValue(mDomain->getMax(), v);
    range.append(v);
  }
  else
  {
    for (const auto & e : mDomain->getValues())
    {
      convertValueIntoTTValue(e, v);
      range.append(v);
    }
  }
  
  mData.set("rangeBounds", range);
  
  return *this;
}

JamomaAddress::BoundingMode JamomaAddress::getBoundingMode() const
{
  return mBoundingMode;
}

Address & JamomaAddress::setBoundingMode(BoundingMode boundingMode)
{
  mBoundingMode = boundingMode;
  
  if (mBoundingMode == BoundingMode::FREE)
    mData.set("rangeClipmode", kTTSym_none);
  else if (mBoundingMode == BoundingMode::CLIP)
    mData.set("rangeClipmode", kTTSym_both);
  else if (mBoundingMode == BoundingMode::WRAP)
    mData.set("rangeClipmode", kTTSym_wrap);
  else if (mBoundingMode == BoundingMode::FOLD)
    mData.set("rangeClipmode", kTTSym_fold);
  
  return *this;
}

bool JamomaAddress::getRepetitionFilter() const
{
  return mRepetitionFilter;
}

Address & JamomaAddress::setRepetitionFilter(bool repetitionFilter)
{
  mRepetitionFilter = repetitionFilter;
  
  mData.set("repetitionsFilter", repetitionFilter);
  
  return *this;
}

# pragma mark -
# pragma mark Callback

Address::ValueCallback JamomaAddress::getValueCallback() const
{
  return mCallback;
}

void JamomaAddress::setValueCallback(Address::ValueCallback callback)
{
  mCallback = callback;
}

# pragma mark -
# pragma mark Expression

bool JamomaAddress::evaluate() const
{
  return false;
}

# pragma mark -
# pragma mark Implementation specific

TTErr JamomaAddress::ValueCallback(const TTValue& baton, const TTValue& value)
{
  JamomaAddress * self = static_cast<JamomaAddress*>(TTPtr(baton[0]));
  TTObject aData = baton[1];
  
  // check data object
  if (aData.instance() == self->mData.instance())
  {
    if (self->mCallback)
    {
      Value * v = self->convertTTValueIntoValue(value, self->mValueType);
      self->mCallback(v);
      
      return kTTErrNone;
    }
  }
  
  return kTTErrGeneric;
}

Value * JamomaAddress::convertTTValueIntoValue(const TTValue& v, Value::Type valueType) const
{
  switch (valueType)
  {
    case Value::Type::IMPULSE :
    {
      return new OSSIA::Impulse();
    }
      
    case Value::Type::BOOL :
    {
      if (v.size() == 1)
        return new OSSIA::Bool(v[0]);
      
      return new OSSIA::Bool();
    }
      
    case Value::Type::INT :
    {
      if (v.size() == 1)
        return new OSSIA::Int(v[0]);
      
      return new OSSIA::Int();
    }
      
    case Value::Type::FLOAT :
    {
      if (v.size() == 1)
        return new OSSIA::Float(v[0]);
      
      return new OSSIA::Float();
    }
      
    case Value::Type::CHAR :
    {
      if (v.size() == 1)
      {
        if (v[0].type() == kTypeString)
        {
          char* c_value = TTString(v[0]).data();
          return new OSSIA::Char(c_value[0]);
        }
      }
      
      return new OSSIA::Char();
    }
      
    case Value::Type::STRING :
    {
      if (v.size() == 1)
      {
        if (v[0].type() == kTypeSymbol)
        {
          TTSymbol s_value = v[0];
          return new OSSIA::String(s_value.c_str());
        }
      }
      
      return new OSSIA::String();
    }
      
    case Value::Type::DESTINATION :
    {
      /*
       if (v.size() == 1)
       {
       if (v[0].type() == kTypeSymbol)
       {
       //! \todo retreive the Address from the symbol
       //TTAddress s_value = v[0];
       
       return new OSSIA::Destination();
       }
       }
       
       return new OSSIA::Destination();
       */
    }
      
    case Value::Type::BEHAVIOR :
    {}
      
    case Value::Type::TUPLE :
    {
      vector<const Value*> t_value;
      
      for (const auto & e : v)
      {
        Value::Type type;
        
        if (e.type() == kTypeBoolean)
        {
          type = Value::Type::BOOL;
        }
        else if (e.type() == kTypeInt8 || e.type() == kTypeUInt8 ||
                 e.type() == kTypeInt16 || e.type() == kTypeUInt16 ||
                 e.type() == kTypeInt32 || e.type() == kTypeUInt32 ||
                 e.type() == kTypeInt64 || e.type() == kTypeUInt64)
        {
          type = Value::Type::INT;
        }
        else if (e.type() == kTypeFloat32 || e.type() == kTypeFloat64)
        {
          type = Value::Type::FLOAT;
        }
        else if (e.type() == kTypeSymbol)
        {
          type = Value::Type::STRING;
        }
        else
        {
          continue;
        }
        
        TTValue t(e);
        t_value.push_back(convertTTValueIntoValue(t, type));
      }
      
      return new OSSIA::Tuple(t_value);
    }
      
    case Value::Type::GENERIC :
    {
      return nullptr; // todo
    }
  }
}

void JamomaAddress::convertValueIntoTTValue(const Value * value, TTValue & v) const
{
  switch (value->getType())
  {
    case Value::Type::IMPULSE :
    {
      break;
    }
      
    case Value::Type::BOOL :
    {
      Bool * b = (Bool*)value;
      v = TTBoolean(b->value);
      break;
    }
      
    case Value::Type::INT :
    {
      Int * i = (Int*)value;
      v = TTInt32(i->value);
      break;
    }
      
    case Value::Type::FLOAT :
    {
      Float * f = (Float*)value;
      v = TTFloat64(f->value);
      break;
    }
      
    case Value::Type::CHAR :
    {
      Char * c = (Char*)value;
      v = TTSymbol(c->value);
      break;
    }
      
    case Value::Type::STRING :
    {
      String * s = (String*)value;
      v = TTSymbol(s->value);
      break;
    }
      
    case Value::Type::DESTINATION :
    {
      Destination * d = (Destination*)value;
      v = TTAddress(buildNodePath(d->value).data());
      break;
    }
      
    case Value::Type::BEHAVIOR :
    {}
      
    case Value::Type::TUPLE :
    {
      Tuple * t = (Tuple*)value;
      
      for (const auto & e : t->value)
      {
        TTValue n;
        convertValueIntoTTValue(e, n);
        
        if (n.size())
          v.append(n[0]);
      }
      
      break;
    }
      
    case Value::Type::GENERIC :
    {
      //! \todo GENERIC case
      break;
    }
  }
}

string JamomaAddress::buildNodePath(shared_ptr<Node> node) const
{
  string path;
  string name = node->getName();
  shared_ptr<Node> parent = node->getParent();
  
  if (parent != nullptr)
  {
    path += buildNodePath(parent);
    if (path != "/")
      path += "/";
    path += name;
  }
  else
  {
    //! \todo use device name
    path = name;
  }
  
  return path;
}
