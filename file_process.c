#include <windows.h>
#include <stdio.h>
#include "win_types.h"

enum{
	FTYPE_UNKOWN=0,
	FTYPE_WAVE,
	FTYPE_MP3
};
int check_wave(unsigned char *buf,int len)
{
	int result=FALSE;
	if(len<44)
		return result;
	if(strncmp(buf,"RIFF",4)!=0)
		return result;
	if(strncmp(buf+8,"WAVE",4)!=0)
		return result;
	if(strncmp(buf+12,"fmt",3)!=0)
		return result;
	if(strncmp(buf+36,"data",4)!=0)
		return result;
	
	result=TRUE;
	return result;
}
int check_file_type(char *fname)
{
	int type=FTYPE_UNKOWN;
	FILE *f;
	f=fopen(fname,"rb");
	if(f){
		unsigned char buf[44]={0};
		int len;
		len=fread(buf,1,sizeof(buf),f);
		if(check_wave(buf,len))
			type=FTYPE_WAVE;
		fclose(f);
	}
	return type;
}
int open_wave_file(char *fname,WEDIT_WINDOW *wedit_win)
{
	int result=FALSE;
	FILE *f;
	f=fopen(fname,"rb");
	if(f){
		unsigned char header[44]={0};
		int *iptr;
		short *sptr;
		int fsize;
		fread(header,1,sizeof(header),f);
		sptr=header+22;
		wedit_win->channels=sptr[0];
		iptr=header+24;
		wedit_win->sample_rate=iptr[0];
		sptr=header+34;
		wedit_win->bits=sptr[0];
		iptr=header+40;
		fsize=iptr[0];
		wedit_win->wave_data=malloc(fsize);
		if(wedit_win->wave_data){
			fread(wedit_win->wave_data,1,fsize,f);
			wedit_win->wave_len=fsize;
		}
		fclose(f);
		result=TRUE;
	}
	return result;
}
int open_file_type(char *fname,int type,WEDIT_WINDOW *wedit_win)
{
	int result=FALSE;
	if(fname==0 || type==0 || wedit_win==0)
		return FALSE;
	switch(type){
	case FTYPE_WAVE:
		{
			result=open_wave_file(fname,wedit_win);
		}
		break;
	}
	InvalidateRect(wedit_win->hwnd,NULL,TRUE);
	return result;
}