#include <windows.h>


LRESULT CALLBACK wave_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	printf("wave:");
	print_msg(msg,lparam,wparam,hwnd);
    switch(msg)
    {
	case WM_CREATE:
		break;
	case WM_CLOSE:
		EndDialog(hwnd,0);
		break;
	}
	return DefMDIChildProc(hwnd, msg, wparam, lparam);

}

int register_wave_win(HINSTANCE hinstance)
{
 	int result=FALSE;
    WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = wave_wnd_proc;
    wc.hInstance     = hinstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
    wc.lpszClassName = "wavewindow";

    if(RegisterClass(&wc))
		result=TRUE;
	return result;
}
