#ifndef __COMMITLOGGERCIRCLE_H__
#define __COMMITLOGGERCIRCLE_H__
# include "Header.h"
# include "LogItem.h"


class CommitLoggerCircle{
public:
	CommitLoggerCircle(U32 init=375);
	bool CIQEmpty() const; //�жϻ��Ƿ�Ϊ��
	void CIQTraverse() const;//������

	U32 CIQLength() const;//���ػ���Ԫ�ظ���

	B8 EnCICircle(LogItem *); //�뻷
	B8 DelCICircle();//����


public:
	U32 size;//��������洢�ռ�,��Ԫ��Ϊ��λ
	U32 front;//��ͷָ��
	U32 rear;//��βָ��
public:
    LogItem **base; //�洢�ռ��ַ

};

#endif //__COMMITLOGGERCIRCLE_H__