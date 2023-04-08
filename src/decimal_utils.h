#ifndef DECIMAL_MODULE_DECIMAL_UTILS_H
#define DECIMAL_MODULE_DECIMAL_UTILS_H

#include <string>
#include "../deps/redismodule/redismodule.h"
#include <boost/multiprecision/cpp_dec_float.hpp>

using std::string;
const int MAX_PREC = 18;


class DecimalUtils {
 public:
  static string add(const string &a, const string &b);

  static string sub(const string &a, const string &b);

  static string mul(const string &a, const string &b);

  static string div(const string &a, const string &b);
};


class RedisDecimalUtils {
 public:
  explicit RedisDecimalUtils(RedisModuleCtx *ctx) : _ctx(ctx) {};

  RedisModuleString *add(const RedisModuleString *a, const RedisModuleString *b);

  RedisModuleString *sub(const RedisModuleString *a, const RedisModuleString *b);

  RedisModuleString *mul(const RedisModuleString *a, const RedisModuleString *b);

  RedisModuleString *div(const RedisModuleString *a, const RedisModuleString *b);

 private:
  RedisModuleCtx *_ctx;
};

#endif //DECIMAL_MODULE_DECIMAL_UTILS_H
