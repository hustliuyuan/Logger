#pragma once//¸Ä³É#ifndef
#include "stdafx.h"
#include "Header.h"
#include "PGRFPaxosEventPkt.h"
#include "LogInterface.h"
#include "LoggerItemQueue.h"
#include "CommitLoggerCircle.h"
#include "LogItem.h"
#include "LoggerState.h"
#include <boost/threadpool.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>

class LogThread;
class LoggerItemQueue;
class CommitLoggerCircle;
class LoggerState;
class Loggerhandle: public LogInterface
{
public:
	Loggerhandle();
	virtual ~Loggerhandle();
	
	virtual void saveItem(const PGRFPaxoesEventPkt &);
	virtual void saveItem(U64 tTxid,U32 event_tType,string kKey,string vValue, B8 cCommitted_flag);
	virtual void writetofile(U32);
 	virtual void init();
	virtual PGRFPaxoesEventPkt getItem(U64 txid);

	virtual void snapshot();
	virtual void recover();
	virtual void readfromfile();
	virtual void terminate();
	virtual int findItem(U64 tixd);
public:
	virtual void setEventCommit(U64 txid);
    void handleWriteLog();
	void fstr_creat();
	void file_close();
	void readfilewithcheck(OFF_P *checkpoint);

private:
	LogItem * creatLogItem(const PGRFPaxoesEventPkt &);
	LogItem * creatLogItem(U64 tTxid,U32 event_tType,string kKey,string vValue, B8 cCommitted_flag);
public:
	LoggerItemQueue *myQueue;
	CommitLoggerCircle *myCircle;
	CommitLoggerCircle *outCircle;
	boost::mutex m_mutexForWrites;
	boost::condition m_condForWrites;
	string filename;
	fstream file;
	U64 countlogitem;
	LogThread *m_logthread_func;
	boost::thread  *m_thread;
	LoggerState *loggerstate;
	U64 txid_beg;
	U64 txid_eof;
	OFF_P position;
	OFF_P checkpoint[FILESIZE];
private:
	bool isRunning;
};
