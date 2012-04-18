#ifndef __LOGITEM_STRUCT_H__
#define __LOGITEM_STRUCT_H__
#include "Header.h"
typedef struct LogItem{
    U64 txid;
	U32 event_type;
	string key;
	string value;
	B8 committed_flag;	
}LogItem;


typedef struct LogPart{
	U64 txid;
	U32 event_type;
	char key[513];
	char value[1025];
	B8 committed_flag;
	U32 flag;//0表示开始或只有一个包，1表示中间包，2表示结束包。
}LogPart;

#endif //__LOGITEM_STRUCT_H__