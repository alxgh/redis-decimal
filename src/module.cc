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

bool ignoreCaseEquals(const std::string& str, const std::string& expect) {
    return std::equal(str.begin(), str.end(), expect.begin(), [](char c1, char c2) {
        return tolower(c1) == tolower(c2);
    });
}

bool redisIgnoreCaseEquals(RedisModuleString *str, const char *expect) {
    return ignoreCaseEquals(std::string(RedisModule_StringPtrLen(str, NULL)), std::string(expect));
}

RedisModuleString *
calculateResult(RedisModuleString *verb, RedisModuleString *val1, RedisModuleString *val2, RedisDecimalUtils rdUtils) {
    // Addition
    if (redisIgnoreCaseEquals(verb, "ADD")) {
        return rdUtils.add(val1, val2);
    }
    // Subtraction
    if (redisIgnoreCaseEquals(verb, "SUB")) {
        return rdUtils.sub(val1, val2);
    }
    // Multiplication
    if (redisIgnoreCaseEquals(verb, "MUL")) {
        return rdUtils.mul(val1, val2);
    }
    // Division
    if (redisIgnoreCaseEquals(verb, "DIV")) {
        return rdUtils.div(val1, val2);
    }
    return nullptr;
}

int handleKeyValue(RedisModuleCtx *ctx, RedisModuleString **argv) {
    RedisModule_AutoMemory(ctx);
    auto *key = (RedisModuleKey *)
            RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ | REDISMODULE_WRITE);
    if (key != nullptr) {
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
    if (val == nullptr) {
        val = RedisModule_CreateString(ctx, "0", 1);
    }

    RedisModuleString *x = argv[3];
    RedisDecimalUtils rdUtils(ctx);
    RedisModuleString *result = calculateResult(argv[2], val, x, rdUtils);

    if (result != nullptr) {
        RedisModuleCallReply *srep =
                RedisModule_Call(ctx, "SET", "ss", argv[1], result);
        ASSERT_NOERROR(ctx, srep)
        if (srep != nullptr) {
            RedisModule_ReplyWithCallReply(ctx, srep);
        }
        return REDISMODULE_OK;
    }

    return RedisModule_WrongArity(ctx);
}

int handleHMap(RedisModuleCtx *ctx, RedisModuleString **argv) {
    RedisModule_AutoMemory(ctx);
    auto *mapKey = (RedisModuleKey *) RedisModule_OpenKey(ctx, argv[1], REDISMODULE_READ | REDISMODULE_WRITE);
    if (mapKey != nullptr) {
        if (RedisModule_KeyType(mapKey) != REDISMODULE_KEYTYPE_HASH &&
            RedisModule_KeyType(mapKey) != REDISMODULE_KEYTYPE_EMPTY) {
            return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
        }
    }

    auto *reply = RedisModule_Call(ctx, "HGET", "ss", argv[1], argv[2]);
    ASSERT_NOERROR(ctx, reply)
    auto *val = RedisModule_CreateStringFromCallReply(reply);
    if (val == nullptr) {
        val = RedisModule_CreateString(ctx, "0", 1);
    }
    auto *x = argv[4];
    RedisDecimalUtils rdUtils(ctx);
    RedisModuleString *result = calculateResult(argv[3], val, x, rdUtils);
    if (result != nullptr) {
        RedisModuleCallReply *srep = RedisModule_Call(ctx, "HSET", "sss", argv[1], argv[2], result);
        ASSERT_NOERROR(ctx, srep)
        if (srep != nullptr) {
            RedisModule_ReplyWithCallReply(ctx, srep);
        }
        return REDISMODULE_OK;
    }
    return RedisModule_WrongArity(ctx);
}

int BigDecimal_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    switch (argc) {
        case 4:
            return handleKeyValue(ctx, argv);
        case 5:
            return handleHMap(ctx, argv);
        default:
            return RedisModule_WrongArity(ctx);
    }
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
