#include "LoggerState.h"

LoggerState::LoggerState()
{
}
LoggerState::LoggerState(U32 queue_size,U32 queue_index,U32 circle_size,U32 circle_index)
{
	set_queue_size(queue_size);
	set_queue_current_index(queue_index);
	set_circle_size(circle_size);
	set_circle_current_index(circle_index);
	log_metafile = "logmetafile.txt";
	metadata = new FileList;
	metadata->txid_beg = 0;//初始化
	metadata->txid_end = 0;
	metadata->logname[25]='\0';
	lastlogfile = "";
	snapshot_version = "";
}

//构建一个file_list结构体，包含文件名，包含的包的开始txid,结束txid，然后将file_list结构体读入logmetafile文件中
void LoggerState::set_file_list(string filename,U64 txid_start,U64 txid_eof)
{
	filename.copy(metadata->logname,filename.size(),0);
	metadata->logname[filename.size()] = '\0';
	metadata->txid_beg = txid_start;
	metadata->txid_end = txid_eof;
	fstream metafile;
	metafile.open(log_metafile,ios_base::app);//追加方式，向文件输出的内容追加到尾部
	while(!metafile)//当文件不存在时，新建一个文件
	{
		cout<<"元文件打开失败"<<endl;
		system("PAUSE");
		metafile.open(log_metafile,ios_base::out);//新建一个文件
		while(!metafile)//文件新建不成功时
		{
			cout<<"新建元文件失败"<<endl;
		    system("PAUSE");
		    metafile.open(log_metafile,ios_base::out);//新建一个文件
		}
		metafile.close();
		metafile.open(log_metafile,ios_base::app);//追加方式，向文件输出的内容追加到尾部
	}
	metafile.seekp(ios_base::end);
	metafile.write(reinterpret_cast<char *>(metadata), sizeof(FileList));
//	metafile.write((char *)&metadata->txid_beg, sizeof(U64));
//	metafile.write((char *)&metadata->txid_end, sizeof(U64));
//	metafile.write(metadata->logname,sizeof(strlen(metadata->logname)));
	metafile.flush();
	metafile.close();
}
void LoggerState::read_metafile()
{
	fstream metafile;
	metafile.open(log_metafile);//以读写方式打开文件
	while(!metafile)
	{
		cout<<"元文件打开失败"<<endl;
		system("PAUSE");
		metafile.open(log_metafile);//以读写方式打开文件
	}
	metafile.seekp(-(int)sizeof(FileList),ios_base::end);
	metafile.read((char* )(metadata), sizeof(FileList));
	metafile.close();
}
void LoggerState::set_queue_current_index(U32 current_index)
{
	queue_current_index = current_index;
}

void LoggerState::set_queue_size(U32 queue_size)
{
	logitem_queue_size = queue_size;
}

void LoggerState::set_circle_size(U32 circle_size)
{
	logitem_circle_size = circle_size;
}

void LoggerState::set_circle_current_index(U32 current_index)
{
	circle_current_index = current_index;
}
	
U32 LoggerState::get_queue_current_index()
{
	return queue_current_index;
}

U32 LoggerState::get_queue_size()
{
	return logitem_queue_size;
}

U32 LoggerState::get_circle_size()
{
	return logitem_circle_size;
}

U32 LoggerState::get_circle_current_index()
{
	return circle_current_index;
}

string LoggerState::get_last_file()
{
	this->read_metafile();
	lastlogfile = string(metadata->logname); 
	//lastlogfile.replace(0,strlen(metadata->logname),metadata->logname);
	return lastlogfile;
}

string LoggerState::get_snapshot_version()
{
	return snapshot_version;
}


LoggerState::~LoggerState()
{
	delete metadata;
}