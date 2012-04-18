#include "LoggerItemQueue.h"

LoggerItemQueue::LoggerItemQueue(U32 init){
    size = init;//��ʼ��С
	base = new ItemNode[size];
	for(U32 i = 0;i<size;i++)
	{
		base[i].logitem = NULL;
	}
	if(!base) exit(1);//�洢����ʧ��
	front = rear = 0;
}

// ���ض���Q��Ԫ�ظ����������еĳ���

U32 LoggerItemQueue::LIQLength() const {
    return ((rear-front+size)%size);
}

//����Ϊ��,�����г���Ϊ0,������
B8 LoggerItemQueue::LIQEmpty() const{
	if(this->LIQLength()==0)
		return true;
	else
		return false;
}


void LoggerItemQueue::LIQTraverse() const {
	U32 length = (rear-front+size)%size;
	cout << "�� ��β �� ��ͷ: ";
	while(length--){
		cout<< base[(front + length)%size].txid <<"   "<< base[(front + length)%size].logitem->event_type<<"  "
			<< base[(front + length)%size].logitem->key <<"  "<<base[(front + length)%size].logitem->value<<"  "
			<< base[(front + length)%size].logitem->committed_flag<< endl;
	}
	cout <<endl;
}

//����ǰ���в��������ڵ�ǰ���е�βԪ��֮��
//����Ԫ�� Ϊ�µĶ���βԪ��,������TRUE,���򷵻�FALSE
B8 LoggerItemQueue::EnLIQueue(U64 aTxid, LogItem * aLogitem )
{
	if((rear+1)%size == front)
	{
		return false;//������
	}
	base[rear].txid = aTxid;
	base[rear].logitem = aLogitem;
	rear = (rear+1)%size;
	return true;
}

B8 LoggerItemQueue::DelLIQueue()
{
	if(front == rear)
	{
		return false;//���п�
	}
	front = (front + 1) %size;
	return true;
}

int LoggerItemQueue::find(U32 left,U32 right, U64 ftxid)
{
	U32 mid;
	while(left <= right)
	{
		mid = (left + right)/2;
		
		if( base[mid].txid < ftxid)
			left = mid +1;
		else if( base[mid].txid >ftxid)
			right =mid -1;
		else 
			return mid;
     }
	return -1;
}

int LoggerItemQueue::setCommit(U64 ftxid)
{
	int tem;
	if(front < rear)
	{
		tem = find( front, rear,ftxid);
		return tem;
	}
	else if ( front > rear) 
	{
		int tem1=find(0,rear,ftxid); //can't use U32, because we compare 0 and -1
		int tem2=find(front,(size-1),ftxid);
		if( tem1 > tem2)
			tem = tem1;
		else
			tem = tem2;
		return tem;
	}
	return -1;
}

