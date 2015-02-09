#define WINVER 0x500
#define _WIN32_WINNT 0x500
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <Commctrl.h>

#include "resource.h"
#include "mdi.h"
HINSTANCE ghinstance=0;
HWND ghmainframe=0,ghmdiclient=0,ghstatusbar=0;
HMENU ghmenu=0;

int move_console()
{
	char title[MAX_PATH]={0}; 
	HWND hcon; 
	GetConsoleTitle(title,sizeof(title));
	if(title[0]!=0){
		hcon=FindWindow(NULL,title);
		SetWindowPos(hcon,0,600,0,800,600,SWP_NOZORDER);
	}
	return 0;
}
void open_console()
{
	char title[MAX_PATH]={0}; 
	HWND hcon; 
	FILE *hf;
	static BYTE consolecreated=FALSE;
	static int hcrt=0;
	
	if(consolecreated==TRUE)
	{
		GetConsoleTitle(title,sizeof(title));
		if(title[0]!=0){
			hcon=FindWindow(NULL,title);
			ShowWindow(hcon,SW_SHOW);
		}
		hcon=(HWND)GetStdHandle(STD_INPUT_HANDLE);
		FlushConsoleInputBuffer(hcon);
		return;
	}
	AllocConsole(); 
	hcrt=_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE),_O_TEXT);

	fflush(stdin);
	hf=_fdopen(hcrt,"w"); 
	*stdout=*hf; 
	setvbuf(stdout,NULL,_IONBF,0);
	GetConsoleTitle(title,sizeof(title));
	if(title[0]!=0){
		hcon=FindWindow(NULL,title);
		ShowWindow(hcon,SW_SHOW); 
		SetForegroundWindow(hcon);
	}
	consolecreated=TRUE;
}
void hide_console()
{
	char title[MAX_PATH]={0}; 
	HANDLE hcon; 
	
	GetConsoleTitle(title,sizeof(title));
	if(title[0]!=0){
		hcon=FindWindow(NULL,title);
		ShowWindow(hcon,SW_HIDE);
		SetForegroundWindow(hcon);
	}
}


HWND create_mainwindow(WNDPROC wndproc,HMENU hmenu,HINSTANCE hinstance)
{
	WNDCLASS wndclass;
	HWND hframe=0;
	char *class_name="mdiframe";
	memset(&wndclass,0,sizeof(wndclass));

	wndclass.style=0; //CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc=wndproc;
	wndclass.hCursor=LoadCursor(NULL, IDC_ARROW);
	wndclass.hInstance=hinstance;
	wndclass.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);
	wndclass.lpszClassName=class_name;
	
	if(RegisterClass(&wndclass)!=0){	
		hframe = CreateWindowEx(WS_EX_ACCEPTFILES,class_name,"wavedit",
			WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_OVERLAPPEDWINDOW, //0x6CF0000
			0,0,
			400,300,
			NULL,           // handle to parent window
			hmenu,
			hinstance,
			NULL);
	}
	return hframe;
}
static WNDPROC old_win_sub_proc=0;
static LRESULT CALLBACK subclass_main_proc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	if(!(msg==WM_ENTERIDLE||msg==WM_SETCURSOR)){
		printf("sub:");
		print_msg(msg,lparam,wparam,hwnd);
	}
	return CallWindowProc(old_win_sub_proc,hwnd,msg,wparam,lparam);
}
int create_status_bar_parts(HWND hwnd,HWND hstatus)
{
	if(hwnd!=0 && hstatus!=0){
		int parts[2]={-1,-1};
		RECT rect={0};
		GetClientRect(hwnd,&rect);
		parts[0]=rect.right/2;
		return SendMessage(hstatus,SB_SETPARTS,2,&parts);
	}
	return FALSE;
}
int set_statusbar_text(HWND hstatus,int part,char *fmt,...)
{
	if(hstatus!=0){
		char str[100]={0};
		va_list va;
		va_start(va,fmt);
		_vsnprintf(str,sizeof(str),fmt,va);
		va_end(va);
		str[sizeof(str)-1]=0;
		return SendMessage(hstatus,SB_SETTEXT,part,str);
	}
	return FALSE;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if(!(msg==WM_ENTERIDLE||msg==WM_SETCURSOR)){
		printf("main:");
		print_msg(msg,lparam,wparam,hwnd);
	}
	switch(msg){
	case WM_CREATE:
		old_win_sub_proc=SetWindowLong(hwnd,GWL_WNDPROC,subclass_main_proc);
		ghstatusbar=CreateStatusWindow(WS_CHILD|WS_VISIBLE,"",hwnd,IDC_STATUS);
		create_status_bar_parts(hwnd,ghstatusbar);
		break;
	case WM_SIZE:
		{
			resize_main_window(hwnd);
		}
		break;
	case WM_COMMAND:
		{
			int item=LOWORD(wparam);
			switch(item){
			case IDC_NEW2:
				printf("new wave\n");
				break;
			}
		}
		break;
	case WM_MENUSELECT:
		{

		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
        break;

	}
	return DefFrameProc(hwnd, ghmdiclient, msg, wparam, lparam);
}
int custom_dispatch(MSG *msg)
{
	HWND hwnd;
	int wparam,lparam;
	wparam=msg->wParam;
	lparam=msg->lParam;
	hwnd=msg->hwnd;
	if(!(msg==WM_ENTERIDLE||msg==WM_SETCURSOR)){
		//printf("xx:");
		//print_msg(msg->message,lparam,wparam,hwnd);
	}
	switch(msg->message){
	case WM_KEYDOWN:
		{
			switch(wparam){
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			}
		}
		break;
	}
	return FALSE;
}
int init_common_ctrls()
{
    INITCOMMONCONTROLSEX ctrls;
	ctrls.dwSize=sizeof(ctrls);
    ctrls.dwICC = ICC_LISTVIEW_CLASSES|ICC_TREEVIEW_CLASSES|ICC_BAR_CLASSES;
	InitCommonControlsEx(&ctrls);
	return 0;
}
int APIENTRY WinMain(HINSTANCE hinstance,HINSTANCE hprevinstance,LPSTR cmdline,int cmdshow)
{
	MSG msg;
	open_console();
	init_common_ctrls();
	ghmenu=LoadMenu(hinstance, MAKEINTRESOURCE(IDR_MENU1));
	ghmainframe=create_mainwindow(WndProc,ghmenu,hinstance);
	ghmdiclient=create_mdiclient(ghmainframe,ghmenu,hinstance,IDC_MDI_CLIENT);
	ShowWindow(ghmainframe,cmdshow);
	UpdateWindow(ghmainframe);
    while(GetMessage(&msg,NULL,0,0)){
		if(!custom_dispatch(&msg)){
			if(!TranslateMDISysAccel(ghmdiclient, &msg)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
    }
	return 0;
}