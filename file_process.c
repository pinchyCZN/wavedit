#include <windows.h>
#include <stdio.h>

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

int open_file_type(char *fname,int type,void *wedit_win)
{
	if(fname==0 || type==0 || wedit_win==0)
		return FALSE;
	switch(type){
	case FTYPE_WAVE:
		{
			
		}
		break;
	}
}