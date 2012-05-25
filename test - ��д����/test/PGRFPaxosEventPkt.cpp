#include "PGRFPaxosEventPkt.h"


PGRFPaxoesEventPkt::PGRFPaxoesEventPkt()
{

}
PGRFPaxoesEventPkt::PGRFPaxoesEventPkt(U64 tTxid,U32 pPaxoesType,string kKey, string vValue, B8 cCommitted_flag)
{
	txid = tTxid;
	paxoesType = pPaxoesType;
	key = kKey;
	value = vValue;
	committed_flag = cCommitted_flag;
}

string PGRFPaxoesEventPkt::toString()const
{
	std::ostringstream strstr;
	strstr<< "txid: "<< txid;
	strstr<< "paxoes type: "<<paxoesType;
	strstr<< "key: "<<key;
	strstr<< "value: "<<value;
	strstr<< "committed_flag: "<<committed_flag;
	return strstr.str();
}

PGRFPaxoesEventPkt::~PGRFPaxoesEventPkt(void)
{
}