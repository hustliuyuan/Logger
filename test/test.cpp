// test.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "Loggerclass.h"
#include <boost/threadpool.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include "LogItem.h"
using namespace std;


void main()
{
	string key;
	U64 i = 0;
	cout<<"input the number 1 is to write logitem "<<endl;
	cout<<"but the number 2 is to read the logitem"<<endl;
	cout<<"but the number 3 is to read directly the logitem"<<endl;
	char get = std::getchar();
	
	int MAX =50000;
	{
		Loggerhandle log;
		log.init();
		string content = "MY";
	   if(get=='1')
	   {
		for(i = 0;i<MAX;i++)
		{
			stringstream str;
			str << i;
			str >> key;
			content += key;
			key += "MYFILE";
			content += "CONTENT";
			PGRFPaxoesEventPkt mypkt_i = PGRFPaxoesEventPkt(i,WRITE,key,content,false);
			key.clear();
			content.clear();
			content += "MY";
			log.saveItem(mypkt_i);
			if(i%2 == 0)
				log.setEventCommit(i);
		}
		log.terminate();
	   }
	   if(get=='2')
	  { 
		  log.readfromfile();
		  log.outCircle->CIQTraverse();
	   }
	}
	if(i==MAX)
		cout<<"测试数据全部执行"<<endl;
	if(get=='3')
	{
		cout << "logpart size is "<<sizeof(LogPart)<<endl;
	fstream logout;
	logout.open("20000.log");
     logout.seekg(0,ios_base::beg);
	while(!logout)
	{
		cout<<"打开元文件失败"<<endl;
		system("PAUSE");
        logout.open("46000.log");
	}
	for(int i=0;i<200;i++)
	{
    // logout.seekg(0,ios_base::beg);
	long pospoint = logout.tellp();
	cout<<"pospoint"<<pospoint<<endl;
//	logout.seekp(-(int)sizeof(LogPart),ios_base::cur);
	LogPart *lp = new LogPart;
//	log.out.write((char*)lp);
	logout.read((char *)(lp), sizeof(LogPart));
/*	logout.read((char *)&lp->txid,sizeof(U64));
	logout.read((char *)&lp->event_type,sizeof(U32));
	logout.read(lp->key,sizeof(char)*513);
	logout.read(lp->value,sizeof(char)*1025);
	logout.read((char *)&(lp->committed_flag),sizeof(bool));
	logout.read((char *)&lp->flag,sizeof(U32));*/
	cout<<lp->txid<<" "<<lp->key<<"  "<<lp->value<<" "<< lp->flag<<endl;
	delete lp;
	}
	}

	std::getchar();
	std::getchar();
}