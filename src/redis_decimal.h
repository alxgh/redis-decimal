#ifndef DECIMAL_MODULE_REDIS_DECIMAL_H
#define DECIMAL_MODULE_REDIS_DECIMAL_H

#include <iostream>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "../deps/redismodule/redismodule.h"

using boost::multiprecision::cpp_dec_float_50;

template<int Prec>
class RedisDecimal : public cpp_dec_float_50 {
 public:
  explicit RedisDecimal(const char *value) : cpp_dec_float_50(std::string(value)) {}

  explicit RedisDecimal(const RedisModuleString *value)
          : cpp_dec_float_50(std::string(RedisModule_StringPtrLen(value, NULL))) {}

  RedisModuleString *toRedisString(RedisModuleCtx *ctx) const {
    std::string str = cpp_dec_float_50::str(*this);
    return RedisModule_CreateString(ctx, str.data(), str.length());
  }

  const RedisDecimal operator+(const RedisDecimal &rhs) const {
    cpp_dec_float_50 result = static_cast<cpp_dec_float_50>(*this) + static_cast<cpp_dec_float_50>(rhs);
    return *static_cast<RedisDecimal<Prec> *>(&result);
  }

  const RedisDecimal operator-(const RedisDecimal &rhs) const {
    cpp_dec_float_50 result = static_cast<cpp_dec_float_50>(*this) - static_cast<cpp_dec_float_50>(rhs);
    return *static_cast<RedisDecimal<Prec> *>(&result);
  }

  const RedisDecimal operator*(const RedisDecimal &rhs) const {
    cpp_dec_float_50 result = static_cast<cpp_dec_float_50>(*this) * static_cast<cpp_dec_float_50>(rhs);
    return *static_cast<RedisDecimal<Prec> *>(&result);
  }

  const RedisDecimal operator/(const RedisDecimal &rhs) const {
    cpp_dec_float_50 result = static_cast<cpp_dec_float_50>(*this) / static_cast<cpp_dec_float_50>(rhs);
    return *static_cast<RedisDecimal<Prec> *>(&result);
  }
};

#endif //DECIMAL_MODULE_REDIS_DECIMAL_H
