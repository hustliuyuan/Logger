#include "Loggerhandle.h"
#include "LogThread.h"
Loggerhandle::Loggerhandle()
{
}

LogItem * Loggerhandle::creatLogItem(const PGRFPaxoesEventPkt  & testPaxoesEventPkt)
{
	LogItem *mylog = new LogItem;
	mylog->txid = testPaxoesEventPkt.txid;
	mylog->event_type = testPaxoesEventPkt.paxoesType;
	mylog->key = testPaxoesEventPkt.key;
	mylog->value = testPaxoesEventPkt.value;
	mylog->committed_flag = false;
	return mylog;
}

LogItem* Loggerhandle::creatLogItem(U64 tTxid,U32 event_tType,string kKey,string vValue, B8 cCommitted_flag)
{
	LogItem * mylog = new LogItem;
	mylog->txid = tTxid;
	mylog->event_type =event_tType;
	mylog->key = kKey;
	mylog->value = vValue;
	mylog->committed_flag = false;
	return mylog;
}

void Loggerhandle::init()
{
     countlogitem = 0;
	 myQueue = new LoggerItemQueue;
	 myCircle = new CommitLoggerCircle;
	 outCircle = new CommitLoggerCircle;
	 loggerstate = new LoggerState(myQueue->size,myQueue->front,myCircle->size,myCircle->front);
	 isRunning = true;
	 txid_eof = 0;
	 m_logthread_func = new LogThread(this,0);
	 m_thread = new boost::thread(*m_logthread_func);

}
void Loggerhandle::saveItem(const PGRFPaxoesEventPkt & pgp)
{
	LogItem *newItem = this->creatLogItem(pgp);
	myQueue->EnLIQueue(pgp.txid,newItem);
}
void Loggerhandle::saveItem(U64 tTxid,U32 event_tType,string kKey,string vValue, B8 cCommitted_flag)
{
	LogItem *newItem = this->creatLogItem(tTxid,event_tType,kKey,vValue,cCommitted_flag);
	myQueue->EnLIQueue(tTxid,newItem);
}
PGRFPaxoesEventPkt Loggerhandle::getItem(U64 txid)
{
	PGRFPaxoesEventPkt myPkt ;
	return myPkt;
}
void Loggerhandle::setEventCommit(U64 txid)
{
    int index = myQueue ->setCommit(txid);
	if( index != -1)
	{
		myQueue->base[index].logitem->committed_flag = true;
	}
	else
	{
			cout << "set" << txid << " Commit failed!"<<endl;
	        return ;
	}
    boost::mutex::scoped_lock lc(m_mutexForWrites);
	myCircle->EnCICircle(myQueue->base[index].logitem); //��ȷ���ύ������־�������־��
	if(myCircle->CIQLength() >= myCircle->size/2)
		m_condForWrites.notify_one();
	int cmp = myQueue->front;
	if( cmp < index)
	{
		for(int i=cmp;i< index;i++)
		{
			delete myQueue->base[i].logitem; //�ͷ�û��COMMIT����ڴ�
			myQueue->base[i].logitem = NULL;
			myQueue->DelLIQueue();//��δ������־������־�����
		}
		myQueue->DelLIQueue();//��������־������־�����
	}
	else if(cmp == index)
	{
		myQueue->DelLIQueue();//��������־������־�����
	}
	else
	{
		//��rear���´�0��ʼ����front��δ�ﵽSIZE,�����ҵ���index��frontС���Ǹ�front��SIZE�Լ�0��index��ָ����Ч
		for(U32 j = myQueue->front; j< myQueue->size;j++)
		{
		  delete myQueue->base[j].logitem; //�ͷ�û��COMMIT����ڴ�
		  myQueue->base[j].logitem = NULL;
		  myQueue->DelLIQueue();//��δ������־������־�����
	    }
		for(int i = 0; i< index;i++) //if index is equal to 0, we can't delete the point
		{
		   if(index != 0)
		   {
	           delete myQueue->base[i].logitem; //�ͷ�û��COMMIT����ڴ�
			   myQueue->base[i].logitem = NULL;
		       myQueue->DelLIQueue();//��δ������־������־�����
		   }
		}
		myQueue->DelLIQueue();//��������־������־�����
	}
	loggerstate->set_queue_current_index(myQueue->front);
	loggerstate->set_circle_current_index(myCircle->front);
}

void Loggerhandle::fstr_creat( )
{
	string temp;
	stringstream ss;
	if(!myCircle->CIQEmpty()||!myQueue->LIQEmpty())//�´�һ���ļ�ʱ��COMMIT������Ϊ�գ�����front�еĽṹ��ָ��ָ���������Ч
	{
		while(!myCircle->CIQEmpty())
		{
			if(myCircle->base[myCircle->front]!=NULL)
			ss << myCircle->base[myCircle->front]->txid;
			txid_beg = myCircle->base[myCircle->front]->txid;
			break;
		}
	}
	else return;
	ss >> temp;
	if(temp.empty())
	{
		return;// if the circle is empty but the queue is not and it has not commit
	}
	filename = temp +".log";
	file.open(filename,ios_base::out);//�Զ�д��ʽ���ļ�
	if(file.fail())
	{
		cout << "�ļ���ʧ��"<<endl;
		system("PAUSE");
		this->fstr_creat();//��ʧ�ܺ����´�
	}

}
void Loggerhandle::file_close()
{
	file.close();
}
void Loggerhandle::writetofile(U32 Length)
{
	//�ж��ļ��Ƿ��
    if(!file.is_open())
	   {
		this->fstr_creat();
	   }
   U32 end = myCircle->front + Length;
   U32 temp;//method two
	 for(U32 i=myCircle->front;i< end;i++)
	 {
		 //���i�Ĵ�С�����˻��Ĵ�С�������´�0��ʼ����endҲ��ȥsize
		 if(i>=myCircle->size)
			{
				i %= myCircle->size;
				end = end -myCircle->size;
		  }
		 //��ֻ��е�key��value��ɸ�С�Ĺ̶����ȵ�logpart��������������ļ�
		 if(myCircle->base[i]->key.size() > 512 || myCircle->base[i]->value.size() > 1024)
		 {
			 int i_p = myCircle->base[i]->key.size()/512 +1;
			 int j_p = myCircle->base[i]->value.size()/1024 +1;
			 if(j_p > i_p)
				 int temp = j_p;
			 else
				 temp = i_p;
			 LogPart *lop = new LogPart[temp];
			 lop[0].flag = 0; //start of the parekt
			 lop[0].txid = myCircle->base[i]->txid;
			 lop[0].event_type = myCircle->base[i]->event_type;
			 lop[0].committed_flag = true;
			 myCircle->base[i]->key.copy(lop[0].key,512,0);
			 lop[0].key[512] = '\0';//��ӽ�����
			 myCircle->base[i]->value.copy(lop[0].value,1024,0);
			 lop[0].value[1024] = '\0';
			 if(temp == 2)
			{ 
				 lop[1].flag = 2;//end of the parket
			     lop[1].txid = myCircle->base[i]->txid;
				 lop[1].event_type = myCircle->base[i]->event_type;
				 lop[1].committed_flag = true;
				 myCircle->base[i]->key.copy(lop[0].key,myCircle->base[i]->key.size()-512,512);
				 lop[1].key[myCircle->base[i]->key.size()-512] = '\0';
			     myCircle->base[i]->value.copy(lop[0].value,myCircle->base[i]->value.size()-1024,1024);
				 lop[1].value[myCircle->base[i]->value.size()-1024] = '0';
			 }
			 else
			 {
				 for(U32 count = 2; count <temp; count++)
					{
						lop[count].flag = 1;
						lop[count].txid = myCircle->base[i]->txid;
				        lop[count].event_type = myCircle->base[i]->event_type;
				        lop[count].committed_flag = true;
				        myCircle->base[i]->key.copy(lop[count].key,512,512*count);
						lop[count].key[512] = '\0';
			            myCircle->base[i]->value.copy(lop[count].value,1024,1024*count);
						lop[count].value[1024] = '\0';
				 }
			 }
			 for(U32 cf = 0; cf<temp;cf++)//�����е�logpart����myCircle�е�һ������ļ�
			 {
				 file.write(reinterpret_cast<char *>(lop),sizeof(LogPart));//һ�ζ���һ��logpart
				 delete lop;
				 if(cf != temp-1)
				 lop++;
				 else;
			 }
		 }
		 else
		 {
			 LogPart *lop = new LogPart;
			 lop->flag = 0; //start of the parekt
			 lop->txid = myCircle->base[i]->txid;
			 lop->event_type = myCircle->base[i]->event_type;
			 lop->committed_flag = true;
			 myCircle->base[i]->key.copy(lop->key,myCircle->base[i]->key.size(),0);
			 lop->key[myCircle->base[i]->key.size()] = '\0';//��ӽ�����
			 myCircle->base[i]->value.copy(lop->value,myCircle->base[i]->value.size(),0);
			 lop->value[myCircle->base[i]->value.size()] = '\0';
			 //��commit���е����ݶ����ļ���
			 file.write(reinterpret_cast<char *>(lop),sizeof(LogPart));
				 delete lop;
		 }
		 //����֮�󽫻�������ָ���logitem�ڴ��ͷŵ���������
		 {
					cout << "delete the struct "<<myCircle->base[i]->txid<<endl; 					
					if(countlogitem == (FILESIZE -1))
						txid_eof = myCircle->base[i]->txid;
					delete myCircle->base[i];
					myCircle->base[i] = NULL;
					myCircle->DelCICircle();
	                loggerstate->set_circle_current_index(myCircle->front);
					countlogitem++;
					if(countlogitem == FILESIZE)
						{
							cout<<"count logitem value is " <<countlogitem<<endl;
							file.flush();
							this->file_close();//����л�
							loggerstate->set_file_list(filename,txid_beg,txid_eof);//�ڹر��ļ�֮ǰ�����ļ���Ϣ���浽logmetafile�ļ���loggerstate->set_file_list(filename,txid_beg,txid_eof);//�ڹر��ļ�֮ǰ�����ļ���Ϣ���浽logmetafile�ļ���
							this->fstr_creat();//�½�һ���ļ������Ŷ���������
							if(!file.is_open())
	                        {
		                        return;//�������û�����ݣ�����������
	                        }
							countlogitem = 0;
					}	    
		 }		 
	 }
	 file.flush();
	
}

void Loggerhandle::readfromfile()
{
	string logfile = loggerstate->get_last_file();
	fstream logout;
	off_t position = (off_t)sizeof(LogPart);
	logout.open(logfile);
	while(!logout)
	{
		cout<<"Ԫ�ļ���ʧ��"<<endl;
		system("PAUSE");
		logout.open(logfile);//�Զ�д��ʽ���ļ�
	}
	logout.seekg(-position,ios_base::end);
	LogPart *lp = new LogPart;
	LogItem *li = new LogItem[outCircle->size];
	for(U32 i=0; i<outCircle->size;i++)
	{
       int temp = (int)i+1;
	   if(i !=0 && ios_base::cur > ios_base::beg)
	   {
		   //�ƶ��ļ�ָ��������Ӧλ��
		   off_t mov = temp%100;
		   logout.seekg(-position*mov,ios_base::end);
		   if(temp>100)
			{
				int count = temp/100;
		        for(int i=0;i<count;i++)
				{
					logout.seekg(-position*100,ios_base::cur);
				}
		   } 
	   }
	   logout.read(reinterpret_cast<char *>(lp), sizeof(LogPart));
	   li->txid = lp->txid;
	   li->event_type = lp->event_type;
	   li->committed_flag = lp->committed_flag;
	   li->key.clear();
	   li->key.append(lp->key);
	   li->value.clear();
	   li->value.append(lp->value);
	   if(lp->flag != 0)
	   {
          while(lp->flag !=0)
		 {
			if(ios_base::cur > ios_base::beg)
			{
			//	logout.seekp(-position*(++temp),ios_base::cur);
			    off_t mov = ++temp%100;
			    logout.seekg(-position*mov,ios_base::end);
		        if(temp>100)
			    {
				   off_t count = temp/100;
		           for(int i=0;i<count;i++)
				   {
				     	logout.seekg(-position*100,ios_base::cur);
				   }
		        }
		        
				logout.read(reinterpret_cast<char *>(lp), sizeof(LogPart));
				li->key.append(lp->key);
				li->value.append(lp->value);
			}
			else
			{
				break;
			}
		 }
        }
	    outCircle->EnCICircle(li);
		li++;
		cout<<"the "<< i+1 << "time"<<endl;
	}
	outCircle->CIQTraverse();//to test
	delete lp;
	delete []li;
}

void Loggerhandle::snapshot()
{}
void Loggerhandle::recover()
{}

Loggerhandle::~Loggerhandle()
{
	//U32 lengthend = myCircle->CIQLength();
	//this->writetofile(lengthend);
	
	isRunning = false;
	//this->file_close();
    m_condForWrites.notify_all();
	m_thread->join();

	delete myQueue;
	delete myCircle;
	delete loggerstate;
	delete outCircle;
}

void Loggerhandle::terminate()
{
   
}

void Loggerhandle::handleWriteLog()
{
	while(isRunning)
	{
		boost::mutex::scoped_lock lc(m_mutexForWrites);
		if(myCircle->CIQLength() < myCircle->size/2)
		{
			cout << "i am waiting to write"<<endl;	
			m_condForWrites.wait(lc);
//			m_condForWrites.timed_wait(lc);//�ȴ�10�룬û���źžͰ�commit���е����ݶ�����־��
		}
		if(isRunning)
		{
			//U32 length = myCircle->size/2;
			U32 length = myCircle->CIQLength();
			this->writetofile(length);
		}
	}
	U32 lengthend = myCircle->CIQLength();
	if(lengthend > 0)
	{
	     txid_eof = myCircle->base[(lengthend+myCircle->front-1)%myCircle->size]->txid;
	 }
	this->writetofile(lengthend);
	if(countlogitem !=FILESIZE)
	this->file_close();//���writetofile�ļ���û�йر��ļ�1
	if(txid_eof!=0&&lengthend>0&&countlogitem !=FILESIZE)
       {
		   loggerstate->set_file_list(filename,txid_beg,txid_eof);;//�ڹر��ļ�֮ǰ�����ļ���Ϣ���浽logmetafile�ļ���
		    cout<<"lengthend  "<<lengthend<<endl;
	}
	
}