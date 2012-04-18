#include "LoggerItemQueue.h"

LoggerItemQueue::LoggerItemQueue(U32 init){
    size = init;//初始大小
	base = new ItemNode[size];
	for(U32 i = 0;i<size;i++)
	{
		base[i].logitem = NULL;
	}
	if(!base) exit(1);//存储分配失败
	front = rear = 0;
}

// 返回队列Q中元素个数，即队列的长度

U32 LoggerItemQueue::LIQLength() const {
    return ((rear-front+size)%size);
}

//队列为空,即队列长度为0,返回真
B8 LoggerItemQueue::LIQEmpty() const{
	if(this->LIQLength()==0)
		return true;
	else
		return false;
}


void LoggerItemQueue::LIQTraverse() const {
	U32 length = (rear-front+size)%size;
	cout << "从 队尾 到 队头: ";
	while(length--){
		cout<< base[(front + length)%size].txid <<"   "<< base[(front + length)%size].logitem->event_type<<"  "
			<< base[(front + length)%size].logitem->key <<"  "<<base[(front + length)%size].logitem->value<<"  "
			<< base[(front + length)%size].logitem->committed_flag<< endl;
	}
	cout <<endl;
}

//若当前队列不满，这在当前队列的尾元素之后，
//插入元素 为新的队列尾元素,并返回TRUE,否则返回FALSE
B8 LoggerItemQueue::EnLIQueue(U64 aTxid, LogItem * aLogitem )
{
	if((rear+1)%size == front)
	{
		return false;//队列满
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
		return false;//队列空
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

