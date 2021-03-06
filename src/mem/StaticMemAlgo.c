//
// Created by zhangchao on 2020/3/8.
//
#include "MemManager.h"
#include "List.h"

uint32_t staticMemSize = 0;
uint32_t leftSize = 0;

DEF_NODE_BEGIN(MemNode)
void* ptr;
uint32_t size;
DEF_NODE_END(MemNode)

MemNode firstMem;
MemNode lastPos;
BaseList busyMem;
BaseList reuseMem;

DEF_NODE_CONVERT_FUNC(MemNode)

BOOL memSizeMatcher (const ListNode * var, const ListNode* match) {
    const MemNode* varMemNode = toMemNodeC(var);
    const MemNode* matchMemNode = toMemNodeC(match);
    return varMemNode->size >= matchMemNode->size ? TRUE: FALSE;
}

struct MemNode* reUseFetch(uint32_t size) {
    MemNode matcher = {NULL};
    matcher.size = size;
    return toMemNode(fetch(&reuseMem, (ListNode*)&matcher, memSizeMatcher));
}

static size_t withMemNode(uint32_t size) {
    return size + sizeof(MemNode) ;
}

static MemNode* doAllocNewMem(uint32_t needSize, uint32_t allocSize) {
    char* newPtr =  lastPos.ptr + lastPos.size;
    MemNode* currentNode = (MemNode*)(newPtr + allocSize );
    currentNode->ptr = newPtr;
    currentNode->size = needSize;
    leftSize -= needSize;
    lastPos.ptr = newPtr;
    lastPos.size = needSize;
    return currentNode;
}

void* staticMalloc(size_t __size) {
    size_t needSize = withMemNode(__size);
    MemNode* currentNode = reUseFetch(needSize);
    if(currentNode == NULL) {
        if(leftSize < needSize ) return NULL;
        currentNode = doAllocNewMem(needSize, __size);
    }
    push_back(&busyMem, (ListNode*)currentNode);
    return currentNode->ptr;
}

static BOOL ptrMatcher (const ListNode * var, const ListNode * match) {
    const MemNode* varMemNode = toMemNodeC(var);
    const MemNode* matchMemNode = toMemNodeC(match);
    return varMemNode->ptr == matchMemNode->ptr? TRUE: FALSE;
}


void  staticFree(void * ptr) {
    MemNode matchNode = { NULL};
    matchNode.ptr = ptr;
    ListNode* toBeFree = fetch(&busyMem, (ListNode*)&matchNode, ptrMatcher);
    toBeFree->next = NULL;
    push_back(&reuseMem, toBeFree);
}

static void* staticCheckMemLeaksPos() {
    if(isEmpty(&busyMem) != TRUE) {
        return toMemNode(busyMem.first)->ptr;
    }
    return NULL;
}

void  StatciRegisterMemFunc(void* buff, size_t size) {
    leftSize = size;
    staticMemSize = size;
    init(&reuseMem);
    init(&busyMem);
    busyMem.last = (ListNode*)&firstMem;
    lastPos.ptr = buff;
    lastPos.size = 0;
}



MemAllocAlgo staticAlgo = {
        staticMalloc, staticFree, StatciRegisterMemFunc,staticCheckMemLeaksPos
};

const MemAllocAlgo* staticMemAlgo() {
    return &staticAlgo;
}
