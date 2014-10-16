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

int copyData(double *src,double *dest,int num)
{
  if(src==NULL||dest==NULL){
    printf("src and dest are NULL\n");
    return -1;
  }
 int i;
 for(i=0;i<num;i++){
  *(dest++)=*(src++);
 }
 return i;
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

void _mfcc_preEmphasize(double *sample,int sampleSize,double factor)
{
 //s2(n) = s(n) - a*s(n-1)
 for(int i=1;i<sampleSize/2;i++)
 {
  sample[i]=sample[i]-factor*sample[i-1];
 }
}

//unitsize=256,
//frame[0]=1~256, 
//frame[1] = (256-64) ~ (256+64) ~ 256 - +256
//frame[2] = (256+256-64) ~ (256+256+64) ~ (256+256) - +254
int _mfcc_frame(double *sample,int sampleSize,int unitsize)
{

 double *mirrordata=new double[sampleSize/2];
 //複製前半段給mirrordata
 copyData(&sample[0],&mirrordata[0],sampleSize/2);

 int m=unitsize/2;//重叠区域的单位大小为正常区的一半
 int frames=2;//加上重叠区后的帧数
 int index=unitsize;//当前添加后的数据的排列序号
 
 double temp[512];
 int i;
 for(i=unitsize; i < sampleSize-unitsize; i+=unitsize){
  //重叠区域数据
  //i=256取出(i-64)~(i+64)給temp
  copyData(&mirrordata[i-m/2],&temp[0],m);
  
  //把(i-64)~(i+64)填到256
  copyData(&temp[0],&sample[index],m);//写回数据
  //index=256+128
  index+=m;
  frames+=1;
  
  
  //原采样数据
  copyData(&mirrordata[i],&temp[0],unitsize);
  copyData(&temp[0],&sample[index],unitsize);//写回数据
  //index=256+128+256
  index+=unitsize;
  frames+=1;
 }

 if(sampleSize%unitsize==0)//刚好等于unitsize大小
 {
  //i = sampleSize-unitsize
  copyData(&mirrordata[i],&temp[0],unitsize);
  copyData(&temp[0],&sample[index],unitsize);//写回数据
 }else{
  copyData(&mirrordata[i],&temp[0],sampleSize%unitsize);
  copyData(&temp[0],&sample[index],sampleSize%unitsize);//写回数据
 }
 frames+=1;
 delete[] mirrordata;
 
 return frames;
}