#include "CommitLoggerCircle.h"


CommitLoggerCircle::CommitLoggerCircle(U32 init){
    size = init;//��ʼ��С
	base = new LogItem*[size];
	for(U32 i = 0;i<size;i++)
		base[i]=NULL;
	if(!base) exit(1);//�洢����ʧ��
	front = rear = 0;
}

// ���ػ���Ԫ�ظ����������еĳ���

U32 CommitLoggerCircle::CIQLength() const {
    return ((rear+size-front)%size);
}

//��Ϊ��,��������Ϊ0,������
B8 CommitLoggerCircle::CIQEmpty() const{
	if(this->CIQLength()==0)
		return true;
	else
		return false;
}


void CommitLoggerCircle::CIQTraverse() const {
	U32 length = (rear-front+size)%size;
	cout << "�� ��β �� ��ͷ: ";
	while(length--){
		cout<< base[(front + length)%size]->txid<<"  "<<base[(front + length)%size]->key<< "  "
			<< base[(front + length)%size]->value<<"  "<< base[(front + length)%size]->committed_flag<<"  "
			<< endl;
	}
	cout <<endl;
}

//����ǰ�����������ڵ�ǰ����βԪ��֮��
//����Ԫ�� Ϊ�µĻ�βԪ��,������TRUE,���򷵻�FALSE
B8 CommitLoggerCircle::EnCICircle( LogItem * aLogitem )
{
	if((rear+1)%size == front)
	{
		return false;//������
	}
	base[rear] = aLogitem;
	rear = (rear+1)%size;
	return true;
}

B8 CommitLoggerCircle::DelCICircle()
{
	if(front == rear)
	{
		return false;//���п�
	}
	front = (front + 1) %size;
	return true;
}

