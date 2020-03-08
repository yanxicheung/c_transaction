//
// Created by zhangchao on 2020/2/11.
//

#ifndef C_TRANSACTION_MEMHELP_H
#define C_TRANSACTION_MEMHELP_H

#include "ExternC.h"
#include "MemManager.h"

EXTERN_STDC_BEGIN

#define STRUCT_ALLOC(type, var)\
type* var = (type*)mallocTc(sizeof(type));

#define ARRAY_ALLOC(type, var, num)\
var = (type*)mallocTc(sizeof(type) * num)

#define CHECK_FREE(ptr)\
if(ptr != NULL) {freeTc(ptr); ptr = NULL;}

EXTERN_STDC_END

#endif //C_TRANSACTION_MEMGUARD_H
