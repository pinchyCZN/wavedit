typedef struct{
	char fname[MAX_PATH];
	float zoom;
	int scrollx,scrolly;
	void *wave_data;
	unsigned __int64 wave_len;
	unsigned __int64 offset;
	unsigned __int64 sel_start,sel_end;
	int channels;
	int sample_rate;
	int bits;
	HWND hwnd;
	HWND hctrl;
	unsigned char *wimage;
	int width,height;
}WEDIT_WINDOW;