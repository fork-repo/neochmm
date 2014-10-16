#include <stdio.h>
#include <stdlib.h>
#include "audiotool.h"

bool ReadWAV(const char* filename,short** pbuffer, long* pwavedata_buffer_size,WAV_HEADER* pwavHeader, WAV_DATA* pwavData)
{
 	FILE *fp=NULL;
    fp=fopen(filename,"rb");
    if (fp == NULL){
      printf("fail to open %s\n", filename);
      return false;
    }
    unsigned short cbSize;
    fread(pwavHeader,sizeof(WAV_HEADER),1,fp); 

    //printf("wavHeader.RIFF=%c%c%c%c\n",wavHeader.RIFF[0],wavHeader.RIFF[1],wavHeader.RIFF[2],wavHeader.RIFF[3]);
    //printf("wavHeader.WAVE=%c%c%c%c\n",wavHeader.WAVE[0],wavHeader.WAVE[1],wavHeader.WAVE[2],wavHeader.WAVE[3]);
    //printf("wavHeader.fmt=%c%c%c%c\n",wavHeader.fmt[0],wavHeader.fmt[1],wavHeader.fmt[2],wavHeader.fmt[3]);
    //printf("Subchunk1Size=%lu\n",wavHeader.Subchunk1Size);
    if (pwavHeader->Subchunk1Size==18){
      fread(&cbSize,1,2,fp);
    }
    fread(pwavData,sizeof(WAV_DATA),1,fp);
    //printf("wavData.Subchunk2ID=%c%c%c%c\n",pwavData->Subchunk2ID[0],wavData.Subchunk2ID[1],wavData.Subchunk2ID[2],wavData.Subchunk2ID[3]);
    //printf("wavData.Subchunk2Size=%lu\n",pwavData->Subchunk2Size);
    short *wavedata_buffer = (short*)malloc(pwavData->Subchunk2Size); //*sizeof(short)?
    fread((void*)wavedata_buffer, pwavData->Subchunk2Size, (size_t)1, fp);
    fclose(fp);
    *pbuffer = wavedata_buffer;
    *pwavedata_buffer_size = pwavData->Subchunk2Size / sizeof(short);
    return true;
}

bool ConvertWAVtoSampleData(const char* filename,double** psampleData,long* psampleData_size,WAV_HEADER* pwavHeader, WAV_DATA* pwavData)
{
	
  short* tmp;
  int ret = 0;
  ret = ReadWAV("neo.wav",&tmp,psampleData_size,pwavHeader, pwavData); 
  double *buffer = (double*)malloc((*psampleData_size)*sizeof(double));
  if (buffer == NULL){
    printf("malloc memory fial\n");
  	return false;
  }

  for (int i=0;i< *psampleData_size;i++){
  	//printf("i=%d\n",i);
    buffer[i] = (double)tmp[i]/32768;
  }
  *psampleData = buffer;
  return ret;
}

void _mfcc_preEmphasize(double *sample,int len,double factor)
{
 //s2(n) = s(n) - a*s(n-1)
 for(int i=1;i<len/2;i++)
 {
  sample[i]=sample[i]-factor*sample[i-1];
 }
}