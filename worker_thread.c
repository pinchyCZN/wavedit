#include <windows.h>
#include <stdio.h>
#include "win_types.h"

extern HWND ghmainframe;
HANDLE event=0;
HANDLE event_idle=0;
HANDLE hworker=0;
int task=0;
char task_info[1024]={0};

enum{
	TASK_OPEN_=1,
	TASK_OPEN_FILE,
	TASK_TEST1
};
int task_test1()
{
	task=TASK_TEST1;
	SetEvent(event);
	return 0;
}
int task_create_win()
{
	task=TASK_OPEN_;
	SetEvent(event);
	return 0;
}
int task_open_file(char *fname)
{
	if(fname){
		_snprintf(task_info,sizeof(task_info),"%s",fname);
		task_info[sizeof(task_info)-1]=0;
		task=TASK_OPEN_FILE;
		SetEvent(event);
	}
	return 0;
}
void __cdecl thread(void *args)
{
	int id;
	HANDLE *event_list=args;
	HANDLE event=0,event_idle=0;
	if(args==0)
		return;
	event=event_list[0];
	event_idle=event_list[1];
	if(event==0 || event_idle==0)
		return;
	printf("worker thread started\n");
	while(TRUE){
		SetEvent(event_idle);
		id=WaitForSingleObject(event,INFINITE);
		if(id==WAIT_OBJECT_0){
			switch(task){
			case TASK_TEST1:
				test_window();
				break;
			case TASK_OPEN_:
				{
					test_window();
				}
				break;
			case TASK_OPEN_FILE:
				{
					char fname[MAX_PATH]={0};
					int type;
					_snprintf(fname,sizeof(fname),"%s",task_info);
					type=check_file_type(fname);
					if(!type){
						MessageBox(ghmainframe,fname,"UKNOWN FILE TYPE",MB_OK|MB_SYSTEMMODAL);
					}
					else{
						void *wedit_win=0;
						create_new_wave_win(&wedit_win);
						open_file_type(fname,type,wedit_win);
					}
				}
				break;
			}
		}
		ResetEvent(event);
	}
	CloseHandle(event);
	hworker=0;
}

int get_guid_str(char *pre,char *str,int str_len)
{
	char guid[20]={0};
	_snprintf(guid,sizeof(guid),"%04X",rand());
	_snprintf(guid,sizeof(guid),"%s%04X",guid,rand());
	_snprintf(guid,sizeof(guid),"%s%04X",guid,rand());
	_snprintf(guid,sizeof(guid),"%s%04X",guid,rand());
	guid[sizeof(guid)]=0;
	_snprintf(str,str_len,"%s%s",pre,guid);
	str[str_len-1]=0;
	return TRUE;
}
int start_worker_thread(HWND hwnd)
{
	char str[80]={0};
	static HANDLE *events[2]={0,0};
	void *args=0;
	get_guid_str("worker thread event:",str,sizeof(str));
	if(event!=0)
		CloseHandle(event);
	event=CreateEvent(NULL,TRUE,FALSE,str);
	if(event==0)
		return FALSE;
	get_guid_str("worker thread idle:",str,sizeof(str));
	if(event_idle!=0)
		CloseHandle(event_idle);
	event_idle=CreateEvent(NULL,TRUE,FALSE,str);
	if(event_idle==0)
		return FALSE;
	events[0]=event;
	events[1]=event_idle;
	args=events;
	hworker=_beginthread(thread,0,args);
	if(hworker==-1){
		MessageBox(hwnd,"Failed to create worker thread","Error",MB_OK|MB_SYSTEMMODAL);
		hworker=0;
	}
	return TRUE;
}