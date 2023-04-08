#include <string>
#include "../deps/redismodule/redismodule.h"
#include "./redis_decimal.h"
#include "./decimal_utils.h"

#define RC_RETURN(rst) \
  RedisModule_ReplyWithString(ctx, rst); \
  return REDISMODULE_OK;

#define ASSERT_NOERROR(ctx, r)                                   \
      if (r == NULL) {                                                      \
          return RedisModule_ReplyWithError(ctx, "ERR reply is NULL");        \
        } else if (RedisModule_CallReplyType(r) == REDISMODULE_REPLY_ERROR) { \
            RedisModule_ReplyWithCallReply(ctx, r);                             \
            return REDISMODULE_ERR;                                             \
          }

bool ignoreCaseEquals(const std::string str, const std::string expect) {
  return std::equal(str.begin(), str.end(), expect.begin(), [](char c1, char c2) {
    return tolower(c1) == tolower(c2);
  });
}

bool redisIgnoreCaseEquals(RedisModuleString *str, const char *expect) {
  return ignoreCaseEquals(std::string(RedisModule_StringPtrLen(str, NULL)), std::string(expect));
}

int validate_precision(int prec) {
  return prec < 0 || MAX_PREC < prec;
}

int BigDecimal_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
  if (argc != 4) {
    return RedisModule_WrongArity(ctx);
  }
  RedisModule_AutoMemory(ctx);
  RedisModuleKey *key = (RedisModuleKey *)
            RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ | REDISMODULE_WRITE);
  if (key != NULL) {
    if (RedisModule_KeyType(key) != REDISMODULE_KEYTYPE_STRING &&
                  RedisModule_KeyType(key) != REDISMODULE_KEYTYPE_EMPTY) {
        return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
      }
  }
  RedisModuleCallReply *rep =
    RedisModule_Call(ctx, "GET", "s", argv[1]);
  RedisModuleString *val;
  ASSERT_NOERROR(ctx, rep)
    val = RedisModule_CreateStringFromCallReply(rep);
  if (val == NULL) {
      val = RedisModule_CreateString(ctx, "0", 1);
  }

  RedisModuleString *x = argv[3];
  RedisDecimalUtils rdUtils(ctx);
  RedisModuleString *result = NULL;
  // Set precision
  // Addition
  if (redisIgnoreCaseEquals(argv[2], "ADD")) {
    result = rdUtils.add(val, x);
  }
  // Subtraction
  if (redisIgnoreCaseEquals(argv[2], "SUB")) {
    result = rdUtils.sub(val, x);
  }
  // Multiplication
  if (redisIgnoreCaseEquals(argv[2], "MUL")) {
    result = rdUtils.mul(val, x);
  }
  // Division
  if (redisIgnoreCaseEquals(argv[2], "DIV")) {
    result = rdUtils.div(val, x);
  }
  if (result != NULL) {
    RedisModuleCallReply *srep =
      RedisModule_Call(ctx, "SET", "ss", argv[1], result);
    ASSERT_NOERROR(ctx, srep)
    if (srep != NULL) {
      RedisModule_ReplyWithCallReply(ctx, srep);
    }
    return REDISMODULE_OK;
  }

  return RedisModule_WrongArity(ctx);
}

#ifdef __cplusplus
extern "C" {
#endif

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
  if (RedisModule_Init(ctx, "RedisDecimal", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR)
    return REDISMODULE_ERR;

  if (RedisModule_CreateCommand(ctx, "decimal", BigDecimal_RedisCommand, "readonly random fast", 0, 0, 0) ==
      REDISMODULE_ERR)
    return REDISMODULE_ERR;

  return REDISMODULE_OK;
}

#ifdef __cplusplus
}
#endif
