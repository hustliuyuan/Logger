#ifndef __LOG_LOGGERSTATE_H__
#define __LOG_LOGGERSTATE_H__
#include "Header.h"

typedef struct FileList{
	U64  txid_beg;
	U64  txid_end;
	char logname[25]; //2的64次方是1.844674407371 * 10^19
	//string snapshot_version;
}FileList;

class LoggerState
{
public:
	LoggerState();
	LoggerState(U32,U32,U32,U32);
	~LoggerState();
public:
	U32 get_queue_current_index();
	U32 get_queue_size();
	U32 get_circle_size();
	U32 get_circle_current_index();
	string get_last_file();
	string get_snapshot_version();
	void set_file_list(string, U64,U64);
	void read_metafile();
	void set_queue_current_index(U32);
	void set_queue_size(U32);
	void set_circle_size(U32);
	void set_circle_current_index(U32);

private:
	U32 logitem_queue_size;
	U32 queue_current_index;
	U32 logitem_circle_size;
	U32 circle_current_index;
	string log_metafile;
	string lastlogfile;
	FileList *metadata;
	string snapshot_version;
};


#endif//__LOG_LOGGERSTATE_H__