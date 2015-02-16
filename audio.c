#include <windows.h>


void CALLBACK waveOutProc(HWAVEOUT hwo,UINT uMsg,DWORD *dwInstance,DWORD *dwParam1,DWORD *dwParam2)
{
}
int open_audio(HWAVEOUT *hwave,WAVEFORMATEX *wfmt)
{
	if(waveOutOpen(hwave,WAVE_MAPPER,wfmt,
		(ULONG)waveOutProc, // Callback Mechanism, a Function callback
		(ULONG)0,                   // User Data To Send to Callback
		CALLBACK_FUNCTION                  // Callback/Notification type (Function)
		)!=MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	return TRUE;
}