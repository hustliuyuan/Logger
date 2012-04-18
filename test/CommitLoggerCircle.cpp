#include "CommitLoggerCircle.h"


CommitLoggerCircle::CommitLoggerCircle(U32 init){
    size = init;//初始大小
	base = new LogItem*[size];
	for(U32 i = 0;i<size;i++)
		base[i]=NULL;
	if(!base) exit(1);//存储分配失败
	front = rear = 0;
}

// 返回环中元素个数，即队列的长度

U32 CommitLoggerCircle::CIQLength() const {
    return ((rear+size-front)%size);
}

//环为空,即环长度为0,返回真
B8 CommitLoggerCircle::CIQEmpty() const{
	if(this->CIQLength()==0)
		return true;
	else
		return false;
}


void CommitLoggerCircle::CIQTraverse() const {
	U32 length = (rear-front+size)%size;
	cout << "从 环尾 到 环头: ";
	while(length--){
		cout<< base[(front + length)%size]->txid<<"  "<<base[(front + length)%size]->key<< "  "
			<< base[(front + length)%size]->value<<"  "<< base[(front + length)%size]->committed_flag<<"  "
			<< endl;
	}
	cout <<endl;
}

//若当前环不满，这在当前环的尾元素之后，
//插入元素 为新的环尾元素,并返回TRUE,否则返回FALSE
B8 CommitLoggerCircle::EnCICircle( LogItem * aLogitem )
{
	if((rear+1)%size == front)
	{
		return false;//环列满
	}
	base[rear] = aLogitem;
	rear = (rear+1)%size;
	return true;
}

B8 CommitLoggerCircle::DelCICircle()
{
	if(front == rear)
	{
		return false;//队列空
	}
	front = (front + 1) %size;
	return true;
}

