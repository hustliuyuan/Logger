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
	 for(int i=0;i<FILESIZE;i++)
		 checkpoint[i]=0;
	 
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
	myCircle->EnCICircle(myQueue->base[index].logitem); //将确认提交过的日志项读入日志环
	if(myCircle->CIQLength() >= myCircle->size/2)
		m_condForWrites.notify_one();
	int cmp = myQueue->front;
	if( cmp < index)
	{
		for(int i=cmp;i< index;i++)
		{
			delete myQueue->base[i].logitem; //释放没有COMMIT项的内存
			myQueue->base[i].logitem = NULL;
			myQueue->DelLIQueue();//将未读入日志环的日志项出队
		}
		myQueue->DelLIQueue();//将读入日志环的日志项出队
	}
	else if(cmp == index)
	{
		myQueue->DelLIQueue();//将读入日志环的日志项出队
	}
	else
	{
		//当rear重新从0开始，而front还未达到SIZE,而查找到的index比front小，那个front到SIZE以及0到index的指针无效
		for(U32 j = myQueue->front; j< myQueue->size;j++)
		{
		  delete myQueue->base[j].logitem; //释放没有COMMIT项的内存
		  myQueue->base[j].logitem = NULL;
		  myQueue->DelLIQueue();//将未读入日志环的日志项出队
	    }
		for(int i = 0; i< index;i++) //if index is equal to 0, we can't delete the point
		{
		   if(index != 0)
		   {
	           delete myQueue->base[i].logitem; //释放没有COMMIT项的内存
			   myQueue->base[i].logitem = NULL;
		       myQueue->DelLIQueue();//将未读入日志环的日志项出队
		   }
		}
		myQueue->DelLIQueue();//将读入日志环的日志项出队
	}
	loggerstate->set_queue_current_index(myQueue->front);
	loggerstate->set_circle_current_index(myCircle->front);
}

void Loggerhandle::fstr_creat( )
{
	string temp;
	stringstream ss;
	if(!myCircle->CIQEmpty()||!myQueue->LIQEmpty())//新打开一个文件时，COMMIT环不能为空，否则front中的结构体指针指向的内容无效
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
	filename = temp +".dat";
	file.open(filename,ios::binary|ios_base::out);//以读写方式打开文件
	if(file.fail())
	{
		cout << "文件打开失败"<<endl;
		system("PAUSE");
		this->fstr_creat();//打开失败后重新打开
	}

}
void Loggerhandle::file_close()
{
	file.close();
}
void Loggerhandle::writetofile(U32 Length)
{
	//判断文件是否打开
    if(!file.is_open())
	   {
		this->fstr_creat();
	   }
    U32 end = myCircle->front + Length;
    
	 for(U32 i=myCircle->front;i< end;i++)
	 {
		 //如果i的大小超过了环的大小，则重新从0开始，且end也减去size
		 if(i>=myCircle->size)
			{
				i %= myCircle->size;
				end = end -myCircle->size;
		  }
		//将logitem，即myCircle中的一项读入文件
		  position = 0;
		  position = file.tellp();
		  checkpoint[countlogitem] = position;
	      file.write(reinterpret_cast<char *>(myCircle->base[i]),sizeof(LogItem));
	      file.write(reinterpret_cast<char *>(&position),sizeof(OFF_P));	 	 
		 //读入之后将环所在项指向的logitem内存释放掉，并计数
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
							this->file_close();//句柄切换
							loggerstate->set_file_list(filename,txid_beg,txid_eof);//在关闭文件之前将此文件信息保存到logmetafile文件中loggerstate->set_file_list(filename,txid_beg,txid_eof);//在关闭文件之前将此文件信息保存到logmetafile文件中
							this->fstr_creat();//新建一个文件，接着读环中数据
							if(!file.is_open())
	                        {
		                        return;//如果环中没有数据，返回主程序
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
	OFF_P offset = -8;
	logout.open(logfile,ios::binary|ios::in);
	while(!logout)
	{
		cout<<"元文件打开失败"<<endl;
		system("PAUSE");
		logout.open(logfile,ios::binary|ios::in);//以读写方式打开文件
	}
	logout.seekg(offset,ios_base::end);
	LogItem *li = new LogItem[outCircle->size];
	for(U32 i=0; i<outCircle->size;i++)
	{
	   if(ios_base::cur > ios_base::beg)
	   {
		   //移动文件指针针向相应位置
		   if(i !=0 ||offset !=-8)
		      logout.seekg(offset-8,ios_base::beg);
		  logout.read(reinterpret_cast<char *>(&offset), sizeof(OFF_P));
	      logout.seekg(offset,ios_base::beg);
		  cout<<logout.tellp()<<"  ";
		  logout.read(reinterpret_cast<char *>(li), sizeof(LogItem));
		  if(i>25)
		  cout<<i<<"  "<< logout.tellp()<<"  ";
			//if( > ios_base::beg)
			{
			//	logout.seekp(-position*(++temp),ios_base::cur);
			//	logout.read(reinterpret_cast<char *>(li), sizeof(LogItem));
			}
        }
	    outCircle->EnCICircle(li);
		cout<<li->txid<<" "<<li->event_type <<" "<<li->key<<"  "<<li->value<<" "<<li->committed_flag<<endl;
		li++;
		
	}
	logout.close();
	outCircle->CIQTraverse();//to test
	delete []li;
}
//要查找的0表示找到，-1表示没找到
int Loggerhandle::findItem(U64 find_txid)
{
	string logfile = loggerstate->get_last_file();
	fstream logout;
	logout.open(logfile,ios::binary|ios::in);
	while(!logout)
	{
		cout<<"元文件打开失败"<<endl;
		system("PAUSE");
		logout.open(logfile,ios::binary|ios::in);//以读写方式打开文件
	}
	LogItem *li = new LogItem;
	logout.read(reinterpret_cast<char *>(li), sizeof(LogItem));
	if(li->txid>find_txid)
	{
		//查找上一个日志文件
	}
	else if (li->txid == find_txid)
	{
		delete li;
		return 0;
	}
	else
	{
		//查找最后一条记录，确定在这个文件范围内
		OFF_P offset = -8;
		logout.seekg(offset,ios_base::end);
		logout.read(reinterpret_cast<char *>(&offset), sizeof(OFF_P));
	    logout.seekg(offset,ios_base::beg);
	    logout.read(reinterpret_cast<char *>(li), sizeof(LogItem));
		if(li->txid<find_txid)//要查找的日志项还没有写进文件
		{
			delete li;
			return -1;
		}
		else if(li->txid == find_txid)
		{
			delete li;
			return 0;
		}
		else
		{
			do
			{
			  logout.seekg(offset-8,ios_base::beg);
		      logout.read(reinterpret_cast<char *>(&offset), sizeof(OFF_P));
	          logout.seekg(offset,ios_base::beg);
		     // cout<<logout.tellp()<<"  ";
		      logout.read(reinterpret_cast<char *>(li), sizeof(LogItem));
			}while(li->txid == find_txid);
		//	cout<<li->txid<<" "<<li->event_type <<" "<<li->key<<"  "<<li->value<<" "<<li->committed_flag<<endl;
			logout.close();
		//	delete li;
			return 0;
		}
	}
	return -1;
}
void Loggerhandle::readfilewithcheck(OFF_P *checkpoint)
{
	string logfile = loggerstate->get_last_file();
	fstream logout;
	logout.open(logfile,ios::binary|ios::in);
	while(!logout)
	{
		cout<<"元文件打开失败"<<endl;
		system("PAUSE");
		logout.open(logfile,ios::binary|ios::in);//以读写方式打开文件
	}
	LogItem *li = new LogItem[outCircle->size];
	U64 count = countlogitem;
	if(checkpoint[count-1]==0)
		{
			cout<<"checkpoint"<<endl;
			system("PAUSE");
           this->readfromfile();
	}
	for(U32 i=0; i<outCircle->size;i++)
	{
	   if(ios_base::cur > ios_base::beg)
	   {
		   //移动文件指针针向相应位置
	      logout.seekg(checkpoint[--count],ios_base::beg);
		  cout<<logout.tellp();
		  logout.read(reinterpret_cast<char *>(li), sizeof(LogItem));
			//if( > ios_base::beg)
			{
			//	logout.seekp(-position*(++temp),ios_base::cur);
			//	logout.read(reinterpret_cast<char *>(li), sizeof(LogItem));
			}
        }
	    outCircle->EnCICircle(li);
		cout<<li->txid <<" "<<li->key<<"  "<<li->value<<endl;
		li++;
		cout<<"the "<< i+1 << "time"<<endl;
	}
	logout.close();
	outCircle->CIQTraverse();//to test
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
//			m_condForWrites.timed_wait(lc);//等待10秒，没有信号就把commit环中的数据读到日志中
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
	this->file_close();//如果writetofile文件中没有关闭文件1
	if(txid_eof!=0&&lengthend>0&&countlogitem !=FILESIZE)
       {
		   loggerstate->set_file_list(filename,txid_beg,txid_eof);;//在关闭文件之前将此文件信息保存到logmetafile文件中
		    cout<<"lengthend  "<<lengthend<<endl;
	}
	
}