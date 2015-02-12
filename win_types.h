typedef struct{
	char fname[MAX_PATH];
	float zoom;
	int scrollx,scrolly;
	short *wave_data;
	__int64 wave_len;
	__int64 offset;
	int stereo;
	int sample_rate;
	HWND hwnd;
	HWND hctrl;
}WEDIT_WINDOW;