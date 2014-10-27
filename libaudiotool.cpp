#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "audiotool.h"

#define M_PI   3.14159265358979323846


bool WAVInfo(const char* filename)
{
 FILE *fp=NULL;
 WAV_HEADER wavHeader;
 WAV_DATA wavData;
    fp=fopen(filename,"rb");
    if (fp == NULL){
      printf("fail to open %s\n", filename);
      return false;
    }
    unsigned short cbSize;
    fread(&wavHeader,sizeof(WAV_HEADER),1,fp); 
    if (wavHeader.Subchunk1Size==18){
      fread(&cbSize,1,2,fp);
    }
    fread(&wavData,sizeof(WAV_DATA),1,fp);
    if (wavHeader.AudioFormat == 1){
      printf("Audio Type is PCM\n");

    }
    if (wavHeader.NumOfChan == 1){
      printf("Audio Channel is Mono\n");
    }else if (wavHeader.NumOfChan == 2){
      printf("Audio Channel is Sterio\n");
    }
    printf("Audio SamplesPerSec is %dHz\n",wavHeader.SamplesPerSec);
    printf("wavHeader.bitsPerSample=%d\n",wavHeader.bitsPerSample);
    
  fclose(fp);
}


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
    short *wavedata_buffer;
    short *temp_buffer = (short*)malloc(pwavData->Subchunk2Size);
    fread((void*)temp_buffer, pwavData->Subchunk2Size, (size_t)1, fp);
    short *ori_temp_buffer = temp_buffer;
    short *ori_wavedata_buffer;
    if (pwavHeader->NumOfChan == 1){
      wavedata_buffer = temp_buffer;
      *pbuffer = wavedata_buffer;
       *pwavedata_buffer_size = pwavData->Subchunk2Size ;
    }else if (pwavHeader->NumOfChan == 2){
      wavedata_buffer = (short*)malloc(pwavData->Subchunk2Size/2);
      ori_wavedata_buffer  = wavedata_buffer;
      *pbuffer = wavedata_buffer;  
      *pwavedata_buffer_size = pwavData->Subchunk2Size/2/sizeof(short);
      memset(wavedata_buffer,0,*pwavedata_buffer_size);
      // each sample has 16bit, sample size = pwavedata_buffer_size/2
      for (int i=0;i< *pwavedata_buffer_size; i++){
        memcpy(wavedata_buffer, temp_buffer, 2);
        //because wavedata_buffer++ equal +2 so i < pwavedata_buffer_size/sizeof(short)
        wavedata_buffer++;
        temp_buffer+=2;
      }
    }else{
       *pwavedata_buffer_size = 0;
       *pbuffer = NULL;
    }
    
    fclose(fp);
    //printf("pwavedata_buffer_size=%lu\n",*pwavedata_buffer_size);
    return true;
}


bool ConvertWAVtoSampleData(const char* filename,float** psampleData,long* psampleData_size,WAV_HEADER* pwavHeader, WAV_DATA* pwavData)
{
	
  short* tmp;
  int ret = 0;
  ret = ReadWAV(filename,&tmp,psampleData_size,pwavHeader, pwavData); 
  float *buffer = (float*)malloc((*psampleData_size)*sizeof(float));
  if (buffer == NULL){
    printf("malloc memory fial\n");
  	return false;
  }

  for (int i=0;i< *psampleData_size;i++){
  	//printf("i=%d\n",i);
    buffer[i] = (float)tmp[i];///32768;
  }
  *psampleData = buffer;
  return ret;
}

//Filter:H(z) = 1-a*z^(-1)
int Preemphasize(float *sample, int sampleN)
{
/* Setting emphFac=0 turns off preemphasis. */
int i;
float emphFac = (float)0.97;

for (i = sampleN-1; i > 0; i--) {
   sample[i] = sample[i] - emphFac * sample[i-1];
}
sample[0] = (float)(1.0 - emphFac) * sample[0];

return(1);
}

int Hamming_window(float *sample, int sampleN)
{
int i;
float* hamm_window ;
float temp = (float)(2 * M_PI / (float)(sampleN-1));

hamm_window = (float*)malloc(sampleN*sizeof(float));

for(i=0 ; i<sampleN ; i++ ) hamm_window[i] = (float)(0.54 - 0.46*cos(temp*i));

for(i=0 ; i<sampleN ;i++ ) sample[i] = hamm_window[i]*sample[i];

free(hamm_window);

return(1);
}

int MfccInitDCTMatrix (float *dctMatrix, int ceporder, int numChannels)
{
    int i, j;
    for (i = 0; i <= ceporder; i++){//12+1
        for (j = 0; j < numChannels; j++){//23
            dctMatrix[i * numChannels + j] = (float) cos (M_PI * (float) i / (float) numChannels * ((float) j + 0.5));
    if(i==0) dctMatrix[i * numChannels + j]*=(float)sqrt(1/(float)numChannels);
    else     dctMatrix[i * numChannels + j]*=(float)sqrt(2/(float)numChannels);
   }
}
return 1;
}

typedef struct { /* mel filter banks */
    int m_lowX;
    int m_centerX;
    int m_highX;
} MfccMelFB;

typedef struct {
    int m_lowX;
    int m_centerX;
    int m_highX;
    float m_sumWeight;
} WfMelFB; /* mel filter bank for noise reduction */

//MfccRound(x): 四捨五入
#define MfccRound(x) ((int)((x)+0.5))
MfccMelFB m_MelFB[23];
float m_MelWeight[512/2+1];
void MfccInitMelFilterBanks (float startingFrequency, float samplingRate, int fftLength, int numChannels)
{
  //
    int i, k;
    float* freq=(float*)malloc(numChannels*sizeof(float)+8);
    int * bin=(int *)malloc(numChannels*sizeof(int)+8);
    float start_mel, fs_per_2_mel;

    /* Constants for calculation */
    freq[0] = startingFrequency;
    start_mel = (float)(2595.0 * log10 (1.0 + startingFrequency / 700.0));
    bin[0] = MfccRound(fftLength * freq[0] / samplingRate);
    freq[numChannels+1] = (float)(samplingRate / 2.0);
    fs_per_2_mel = (float)(2595.0 * log10 (1.0 + (samplingRate / 2.0) / 700.0));
    bin[numChannels+1] = MfccRound(fftLength * freq[numChannels+1] / samplingRate);

/* Calculating mel-scaled frequency and the corresponding FFT-bin */
    /* number for the lower edge of the band                          */
//Mel to frequency and then freq to bin(bin is what??)
    for (k = 1; k <= numChannels; k++) {
        freq[k] = (float)(700 * (pow (10, (start_mel + (float) k / (numChannels + 1) * (fs_per_2_mel - start_mel)) / 2595.0) - 1.0));
        bin[k] = MfccRound(fftLength * freq[k] / samplingRate);
    }

/* This part is never used to compute MFCC coefficients */
/* but initialized for completeness                     */
//    float* m_MelWeight = (float*)malloc(fftLength*2+4);
    for(i = 0; i<bin[0]; i++){
      m_MelWeight[i]=0;
    }
    m_MelWeight[fftLength/2]=1;

//    WfMelFB m_MelFB[23];
/* Initialize low, center, high indices to FFT-bin */
    for (k = 0; k <= numChannels; k++) {
      if(k<numChannels){
          m_MelFB[k].m_lowX=bin[k];
          m_MelFB[k].m_centerX=bin[k+1];
          m_MelFB[k].m_highX=bin[k+2];
      }
      for(i = bin[k]; i<bin[k+1]; i++){
          m_MelWeight[i]=(i-bin[k]+1)/(float)(bin[k+1]-bin[k]+1);
      }
    }


  free(freq);
  free(bin);
  return;
}

void PRFFT_NEW(float *a, float *b, int m, int n_pts, int iff)
{
int l,lmx,lix,lm,li,j1,j2,ii,jj,nv2,nm1,k,lmxy,n;
float scl,s,c,arg,T_a,T_b;

n = 1 << m;
for( l=1 ; l<=m ; l++ ) {
   lmx = 1 << (m - l) ;
   lix = 2 * lmx ;
   scl = 2 * (float)M_PI/(float)lix ;
  
   if(lmx < n_pts) lmxy = lmx ;
   else lmxy = n_pts ;
   for( lm = 1 ; lm <= lmxy ; lm++ ) {
    arg=((float)(lm-1)*scl) ;
    c = (float)cos(arg) ;
    s = iff * (float)sin(arg) ;
   
    for( li = lix ; li <= n ; li += lix ) {
     j1 = li - lix + lm ;
     j2 = j1 + lmx ;
     if(lmxy != n_pts ) {
      T_a=a[j1-1] - a[j2-1] ;
      /* T_a : real part */
      T_b=b[j1-1] - b[j2-1] ;
      /* T_b : imaginary part */
      a[j1-1] = a[j1-1] + a[j2-1] ;
      b[j1-1] = b[j1-1] + b[j2-1] ;
      a[j2-1] = T_a*c + T_b*s ;
      b[j2-1] = T_b*c - T_a*s ;
     } else{
      a[j2-1] = a[j1-1]*c + b[j1-1]*s ;
      b[j2-1] = b[j1-1]*c - a[j1-1]*s ;
     }
    }
   }
}
nv2 = n/2 ;
nm1 = n - 1 ;
jj = 1 ;

for( ii = 1 ; ii <= nm1 ;) {
   if( ii < jj ) {
    T_a = a[jj-1] ;
    T_b = b[jj-1] ;
    a[jj-1] = a[ii-1] ;
    b[jj-1] = b[ii-1] ;
    a[ii-1] = T_a ;
    b[ii-1] = T_b ;
   }
   k = nv2 ;
   while( k < jj ) {
    jj = jj - k ;
    k = k/2 ;
   }
   jj = jj + k ;
   ii = ii + 1 ;
}
if(iff == (-1)){
   for( l=0 ; l<n ; l++ ) {
    a[l]/=(float)n;
    b[l]/=(float)n;
   }
}
}

float LogE(float x)
{
if(x>1) return log(x);
else return 0;
}


void MfccMelFilterBank (float *sigFFT, int numChannels, float* output, int normalize)
{
    float sum, wsum;
    int i, k;
MfccMelFB *melFB;

    for (k=0;k<numChannels;k++){
   melFB=&m_MelFB[k];
        sum = sigFFT[melFB->m_centerX];
   wsum=1;
        for (i = melFB->m_lowX; i < melFB->m_centerX; i++){
            sum += m_MelWeight[i] * sigFFT[i];
    wsum += m_MelWeight[i];
   }
   for (i = melFB->m_centerX+1; i <= melFB->m_highX; i++){
            sum += (1 - m_MelWeight[i-1]) * sigFFT[i];
    wsum += (1 - m_MelWeight[i-1]);
   }
        output[k] = sum;
   if(normalize) {
//    assert(wsum>0);
    output[k] /= wsum;
   }
    }
    return;
}


int _filterbank_basic(float *sample, int frameSize, float *filter_bank, int fborder, int fftSize, int cep_smooth, int cepFilterLen)
{

int i;
float* a=(float*)malloc(fftSize*4);
float* b=(float*)malloc(fftSize*4);

int uiLogFFTSize = (int)(log((double)fftSize)/log((double)2)+0.5);
//32000?
MfccInitMelFilterBanks((float)64.0, (float)22050, fftSize, fborder);

for(i=0;i<frameSize;i++){ a[i] = sample[i]; b[i]=0; }
for(i=frameSize;i<fftSize;i++) a[i] = b[i] = 0;

PRFFT_NEW( &a[0], &b[0], uiLogFFTSize, frameSize, 1); 

for(i=0;i<fftSize;i++){
   a[i] = a[i]*a[i] + b[i]*b[i];
   b[i] = 0.0;
}
MfccMelFilterBank (&a[0], fborder, &filter_bank[0], 1); 
for (i = 0; i < fborder; i++) 
   filter_bank[i] = 0.5*LogE(filter_bank[i]);
  
free(a);
free(b);

return(1);
}

void MfccDCT(float *x, float *dctMatrix, int ceporder, int numChannels, float *mel_cep)
{
    int i, j;
    for (i = 0; i <= ceporder; i++) {
        mel_cep[i] = 0.0;
        for (j = 0; j < numChannels; j++){
            mel_cep[i] += x[j] * dctMatrix[i * numChannels + j];
   }
    } 
    return;
}

int _mel_cepstrum_basic(float *sample, int frameSize, float *mel_cep, int fborder, int ceporder, int fft_size)
{
  int i;
   float* filter_bank = (float*)malloc(fborder*sizeof(float));
   float f;
   float *m_dctMatrix = (float*) malloc(sizeof(float) * ((12+1)*23));
   MfccInitDCTMatrix(m_dctMatrix, ceporder, fborder);
   _filterbank_basic(sample, frameSize, filter_bank, fborder, fft_size, 0, 0);
   MfccDCT(filter_bank, m_dctMatrix, ceporder, fborder, mel_cep);
    free(filter_bank);
//   /* scale down to be consistent with other kinds of cepstrum coefficients */
   //f=fborder/(float)fft_size;
  // for(i=0;i<=ceporder;i++) mel_cep[i]*=f;
  // for(i=0;i<=ceporder;i++) {
  //  printf("mel_cep[%d]=%f\n",i,mel_cep[i]);
  // }
  return 1;
}

int readMMFCfromWAV(const char* filename,float ***pall_mel_cep,int cepOrder, int fbOrder)
{
    WAV_HEADER wavHeader;
    WAV_DATA wavData;
    float* samples;
    float small_data[512];
    long samplesize;
    bool ret;
    float* mel_cep = (float*) malloc(sizeof(float) * (cepOrder + 1));
    ret = ConvertWAVtoSampleData(filename,&samples,&samplesize,&wavHeader, &wavData);
    int frame_size = samplesize/256 -1;
    float** all_mel_cep = (float**) malloc(sizeof(float*) * frame_size);
    for (int c=0;c < frame_size;c++){
        all_mel_cep[c] = (float*) malloc(sizeof(float) * (cepOrder + 1));
    }
    for (int c=0;c < frame_size;c++){
        for(int i=0;i<512;i++){
            small_data[i] = samples[(256*c)+i];
        }
         Preemphasize(small_data,512);
         Hamming_window(small_data, 512);
        _mel_cepstrum_basic(small_data, 512, mel_cep, fbOrder, cepOrder, 512);
        for(int i=0;i<(cepOrder + 1);i++) {
           all_mel_cep[c][i] = mel_cep[i];
        }
    }
    *pall_mel_cep = all_mel_cep;
    /*
     for (int c=0;c < frame_size;c++){
         printf("m%d[ ",c);
         for(int i=0;i<(cepOrder + 1);i++) {
             printf("%.2f ",all_mel_cep[c][i]);
         }
          printf("]\n");
     }
     */
    // printf("frame_size=%d\n",frame_size);
     return frame_size;
}