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
	metadata->txid_beg = 0;//��ʼ��
	metadata->txid_end = 0;
	metadata->logname[25]='\0';
	lastlogfile = "";
	snapshot_version = "";
}

//����һ��file_list�ṹ�壬�����ļ����������İ��Ŀ�ʼtxid,����txid��Ȼ��file_list�ṹ�����logmetafile�ļ���
void LoggerState::set_file_list(string filename,U64 txid_start,U64 txid_eof)
{
	filename.copy(metadata->logname,filename.size(),0);
	metadata->logname[filename.size()] = '\0';
	metadata->txid_beg = txid_start;
	metadata->txid_end = txid_eof;
	fstream metafile;
	metafile.open(log_metafile,ios_base::app);//׷�ӷ�ʽ�����ļ����������׷�ӵ�β��
	while(!metafile)//���ļ�������ʱ���½�һ���ļ�
	{
		cout<<"Ԫ�ļ���ʧ��"<<endl;
		system("PAUSE");
		metafile.open(log_metafile,ios_base::out);//�½�һ���ļ�
		while(!metafile)//�ļ��½����ɹ�ʱ
		{
			cout<<"�½�Ԫ�ļ�ʧ��"<<endl;
		    system("PAUSE");
		    metafile.open(log_metafile,ios_base::out);//�½�һ���ļ�
		}
		metafile.close();
		metafile.open(log_metafile,ios_base::app);//׷�ӷ�ʽ�����ļ����������׷�ӵ�β��
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
	metafile.open(log_metafile);//�Զ�д��ʽ���ļ�
	while(!metafile)
	{
		cout<<"Ԫ�ļ���ʧ��"<<endl;
		system("PAUSE");
		metafile.open(log_metafile);//�Զ�д��ʽ���ļ�
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