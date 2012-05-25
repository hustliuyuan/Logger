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
	bool LIQEmpty() const; //�ж϶����Ƿ�Ϊ��
	void LIQTraverse() const;//��������

	U32 LIQLength() const;//���ض�����Ԫ�ظ���

	B8 EnLIQueue(U64, LogItem *); //���
	B8 DelLIQueue();//����
	void binaryFind(U64 txid);
	int setCommit(U64 txid);
private:
	int find(U32 left,U32 right, U64 ftxid);
public:
	U32 size;//��������洢�ռ�,��Ԫ��Ϊ��λ
	U32 front;//��ͷָ��
	U32 rear;//��βָ��
public:
    ItemNode *base; //�洢�ռ��ַ

};