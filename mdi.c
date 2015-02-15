#include <windows.h>
#include "resource.h"
extern HINSTANCE ghinstance;
extern HWND ghmainframe,ghmdiclient,ghstatusbar;

const char *WAVEDITWINCLASS="wavewindow";

#include "win_types.h"
WEDIT_WINDOW wedit_windows[40]={0};

int find_wedit_win(HWND hwnd,WEDIT_WINDOW **win)
{
	int i;
	for(i=0;i<sizeof(wedit_windows)/sizeof(WEDIT_WINDOW);i++){
		if(hwnd==wedit_windows[i].hwnd){
			*win=&wedit_windows[i];
			return TRUE;
		}
	}
	return FALSE;
}
int draw_wave_win(HWND hwnd,DRAWITEMSTRUCT *di)
{
	WEDIT_WINDOW *win=0;
	if(find_wedit_win(hwnd,&win)){
		if(win->wave_data && win->wave_len){
			RECT *rect=&di->rcItem;
			int i;
			float dlen,xstride,ystride;
			float width,height;
			HBRUSH brush;
			HGDIOBJ hold;
			width=rect->right-rect->left;
			height=rect->bottom-rect->top;
			dlen=win->wave_len;
			if(width<=0 || height<=0)
				return FALSE;
			xstride=dlen/width;
			if(xstride<1)
				xstride=1;
			if(height>0)
				ystride=0xFFFF/height;
			else
				ystride=0xFFFF;
			MoveToEx(di->hDC,0,(rect->bottom-rect->top)/2,NULL);
			FillRect(di->hDC,&di->rcItem,(HBRUSH)COLOR_BACKGROUND+1);
			brush=CreateSolidBrush(0xFF00);
			hold=SelectObject(di->hDC,brush);
			for(i=0;i<dlen;i+=xstride/2){
				short *wav=win->wave_data+i;
				int pos;
				if(i/2>=dlen)
					break;
				pos=wav[0];
				if(pos!=0)
					pos=pos;
				LineTo(di->hDC,i,(pos/ystride)+(height/2));
			}
			if(hold)
				SelectObject(di->hDC,hold);
			if(brush)
				DeleteObject(brush);
		}
	}
}
int create_wedit_win(HWND hwnd,HINSTANCE hinstance,WEDIT_WINDOW *win)
{
	RECT rect={0};
	GetClientRect(hwnd,&rect);
    win->hctrl=CreateWindowEx(WS_EX_TOPMOST,"button","", 
      WS_TABSTOP|WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,
        rect.left, rect.top, rect.right, rect.bottom, hwnd, IDC_WAVE_WIN, hinstance, 0);
}
LRESULT CALLBACK WaveMDIWinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if(!(msg==WM_ENTERIDLE||msg==WM_SETCURSOR)){
		printf("mdi:");
		print_msg(msg,lparam,wparam,hwnd);
	}
	switch(msg){
	case WM_CREATE:
		{
			WEDIT_WINDOW *win=0;
			LPCREATESTRUCT pcs = (LPCREATESTRUCT)lparam;
			LPMDICREATESTRUCT pmdics = (LPMDICREATESTRUCT)(pcs->lpCreateParams);
			win=pmdics->lParam;
			if(win!=0){
				win->hwnd=hwnd;
			}
			create_wedit_win(hwnd,ghinstance,win);

		}
        break;
	case WM_SIZE:
		{
			RECT rect={0};
			HWND hbutton=GetDlgItem(hwnd,IDC_WAVE_WIN);
			GetClientRect(hwnd,&rect);
			SetWindowPos(hbutton,NULL,rect.left,rect.top,rect.right,rect.bottom,SWP_NOZORDER);
		}
		break;
	case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT *di=lparam;
			if(di!=0 && di->CtlType==ODT_BUTTON){
				draw_wave_win(hwnd,di);
				return TRUE;
			}
		}
		break;
	}
	return DefMDIChildProc(hwnd, msg, wparam, lparam);
}

int register_mdi_classes(HINSTANCE hinstance)
{
 	int result=FALSE;
    WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WaveMDIWinProc;
    wc.hInstance     = hinstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
    wc.lpszClassName = WAVEDITWINCLASS;

    if(RegisterClass(&wc))
		result=TRUE;
	return result;
}
HWND create_mdiclient(HWND hwnd,HMENU hmenu,HINSTANCE hinstance,int client_id)
{
	CLIENTCREATESTRUCT MDIClientCreateStruct;
	HWND hmdiclient=0;
	MDIClientCreateStruct.hWindowMenu   = GetSubMenu(hmenu,5);
	MDIClientCreateStruct.idFirstChild  = 50000;
	hmdiclient = CreateWindow("MDICLIENT",NULL,
		WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VSCROLL|WS_HSCROLL|WS_VISIBLE,
		0,0,0,0,
		hwnd,
		(HMENU)client_id,
		hinstance,
		(LPVOID*)&MDIClientCreateStruct);
	return hmdiclient;
}
int create_wavedit_window(HWND hmdiclient,WEDIT_WINDOW *win)
{
	int style,handle;
	MDICREATESTRUCT cs;
	char title[256]={0};
	style = MDIS_ALLCHILDSTYLES;
	cs.cx=cs.cy=cs.x=cs.y=CW_USEDEFAULT;
	cs.szClass=WAVEDITWINCLASS;
	cs.szTitle=title;
	cs.style=style;
	cs.hOwner=ghinstance;
	cs.lParam=win;
	handle=SendMessage(hmdiclient,WM_MDICREATE,0,&cs);
	return handle;
}

int create_new_wave_win(WEDIT_WINDOW **win)
{
	if(win){
		create_wavedit_window(ghmdiclient,&wedit_windows[0]);
		*win=&wedit_windows[0];
		return TRUE;
	}
	return FALSE;
}
int test_window()
{

	create_wavedit_window(ghmdiclient,&wedit_windows[0]);
}

int process_drop(HWND hwnd,HANDLE hdrop)
{
	int i,count,is_file=FALSE;
	char str[MAX_PATH];

	count=DragQueryFile(hdrop,-1,NULL,0);
	for(i=0;i<count;i++){
		str[0]=0;
		DragQueryFile(hdrop,i,str,sizeof(str));
		if(str[0]==0)
			continue;
		if(!is_path_directory(str)){
			is_file=TRUE;
			break;
		}
	}
	DragFinish(hdrop);
	if(is_file)
		task_open_file(str);
	return 0;
}