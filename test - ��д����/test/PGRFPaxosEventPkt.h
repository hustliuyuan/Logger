#pragma once
#include "Header.h"
class PGRFPaxoesEventPkt
{
	public:
		PGRFPaxoesEventPkt();
		PGRFPaxoesEventPkt(U64 txid,U32 paxoesType,string key, string value, B8 committed_flag);

           virtual string					toString() const ;

    public:
		virtual ~PGRFPaxoesEventPkt(void);
public:
	U64 txid;
	U32 paxoesType;
	string key;
	string value;
	B8 committed_flag;
};