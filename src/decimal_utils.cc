#include "./decimal_utils.h"

using boost::multiprecision::cpp_dec_float_50;
#define DECIMAL_D cpp_dec_float_50

#define RESULT(n) return n.str();


string DecimalUtils::add(const string &a, const string &b) {
  DECIMAL_D x(a), y(b);
  x += y;
  RESULT(x)
}

string DecimalUtils::sub(const string &a, const string &b) {
  DECIMAL_D x(a), y(b);
  x -= y;
  RESULT(x)
}

string DecimalUtils::mul(const string &a, const string &b) {
  DECIMAL_D x(a), y(b);
  x *= y;
  RESULT(x)
}

string DecimalUtils::div(const string &a, const string &b) {
  DECIMAL_D x(a), y(b);
  x /= y;
  RESULT(x)
}


RedisModuleString *
RedisDecimalUtils::add(const RedisModuleString *a, const RedisModuleString *b) {
  string rst = DecimalUtils::add(RedisModule_StringPtrLen(a, NULL), RedisModule_StringPtrLen(b, NULL));
  return RedisModule_CreateString(this->_ctx, rst.data(), rst.length());
}

RedisModuleString *
RedisDecimalUtils::sub(const RedisModuleString *a, const RedisModuleString *b) {
  string rst = DecimalUtils::sub(RedisModule_StringPtrLen(a, NULL), RedisModule_StringPtrLen(b, NULL));
  return RedisModule_CreateString(this->_ctx, rst.data(), rst.length());
}

RedisModuleString *
RedisDecimalUtils::mul(const RedisModuleString *a, const RedisModuleString *b) {
  string rst = DecimalUtils::mul(RedisModule_StringPtrLen(a, NULL), RedisModule_StringPtrLen(b, NULL));
  return RedisModule_CreateString(this->_ctx, rst.data(), rst.length());
}

RedisModuleString *
RedisDecimalUtils::div(const RedisModuleString *a, const RedisModuleString *b) {
  string rst = DecimalUtils::div(RedisModule_StringPtrLen(a, NULL), RedisModule_StringPtrLen(b, NULL));
  return RedisModule_CreateString(this->_ctx, rst.data(), rst.length());
}
