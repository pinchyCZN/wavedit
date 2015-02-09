#include <windows.h>


int register_mdi_classes(HINSTANCE hinstance,WNDPROC wnd_proc)
{
 	int result=FALSE;
    WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = wnd_proc;
    wc.hInstance     = hinstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
    wc.lpszClassName = "wavewindow";

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