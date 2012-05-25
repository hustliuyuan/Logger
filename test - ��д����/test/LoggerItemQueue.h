#pragma once
# include "Header.h"
# include "LogItem.h"
typedef struct ItemNode{
	U64 txid;
	LogItem *logitem;
}ItemNode;


class LoggerItemQueue{
public:
	LoggerItemQueue(U32 init=355);
	bool LIQEmpty() const; //判断队列是否为空
	void LIQTraverse() const;//遍历队列

	U32 LIQLength() const;//返回队列中元素个数

	B8 EnLIQueue(U64, LogItem *); //入队
	B8 DelLIQueue();//出队
	void binaryFind(U64 txid);
	int setCommit(U64 txid);
private:
	int find(U32 left,U32 right, U64 ftxid);
public:
	U32 size;//允许的最大存储空间,以元素为单位
	U32 front;//队头指针
	U32 rear;//队尾指针
public:
    ItemNode *base; //存储空间基址

};