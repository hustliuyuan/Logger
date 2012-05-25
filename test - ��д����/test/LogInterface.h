#ifndef __LOG_LOGINTERFACE_H__
#define __LOG_LOGINTERFACE_H__
#include "Header.h"
class LogInterface
{
public:
//	 LogInterface();
//	 LogInterface(const PGRFPaxoesEventPkt &);
//	 LogInterface(U64 txid,U32 event_type,string key,string value, B8 committed_flag);
	virtual ~LogInterface()=0 {};

	virtual void saveItem(const PGRFPaxoesEventPkt &) = 0;//
	virtual void saveItem(U64 tTxid,U32 event_tType,string kKey,string vValue, B8 cCommitted_flag) = 0;
	virtual void writetofile(U32) = 0;
	virtual PGRFPaxoesEventPkt getItem(U64 txid)= 0;
	virtual void snapshot() = 0;
	virtual void recover() = 0;
	virtual void setEventCommit(U64 txid) = 0;
	virtual int  findItem(U64 txid) = 0;
	virtual void readfromfile() = 0;
	virtual void terminate() = 0;

};
#endif //__LOG_LOGINTERFACE_H__