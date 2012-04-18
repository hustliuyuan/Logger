#ifndef __COMMITLOGGERCIRCLE_H__
#define __COMMITLOGGERCIRCLE_H__
# include "Header.h"
# include "LogItem.h"


class CommitLoggerCircle{
public:
	CommitLoggerCircle(U32 init=375);
	bool CIQEmpty() const; //判断环是否为空
	void CIQTraverse() const;//遍历环

	U32 CIQLength() const;//返回环中元素个数

	B8 EnCICircle(LogItem *); //入环
	B8 DelCICircle();//出环


public:
	U32 size;//允许的最大存储空间,以元素为单位
	U32 front;//队头指针
	U32 rear;//队尾指针
public:
    LogItem **base; //存储空间基址

};

#endif //__COMMITLOGGERCIRCLE_H__