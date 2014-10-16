#pragma once
#include <fstream> //for std::ostream
#include <vector>

typedef struct 
{
    unsigned char       RIFF[4];        // RIFF Header --> "RIFF" = 0x46464952
    unsigned long       ChunkSize;      // RIFF Chunk Size  
    unsigned char       WAVE[4];        // WAVE Header --> "WAVE" = 0x45564157
    unsigned char       fmt[4];         // FMT header  --> "fmt " = 0x20746d66
    unsigned long       Subchunk1Size;  // Size of the fmt chunk                                
    unsigned short      AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM 
    unsigned short      NumOfChan;      // Number of channels 1=Mono 2=Sterio                   
    unsigned long       SamplesPerSec;  // Sampling Frequency in Hz, 8000Hz,11025Hz,22050Hz                           
    unsigned long       bytesPerSec;    // bytes per second(bps)
    unsigned short      blockAlign;     // 2=16-bit mono, 4=16-bit stereo 
    unsigned short      bitsPerSample;  // Number of bits per sample       
}WAV_HEADER; 
typedef struct 
{
  unsigned char       Subchunk2ID[4]; // "data"  string   --> "data" = 0x61746164
  unsigned long       Subchunk2Size;  // Sampled data length    
}WAV_DATA;

extern bool ReadWAV(const char* filename,short** pbuffer, long* pwavedata_buffer_size,WAV_HEADER* pwavHeader, WAV_DATA* pwavData);
extern bool ConvertWAVtoSampleData(const char* filename,double** sampleData,long* psampleData_size,WAV_HEADER* pwavHeader, WAV_DATA* pwavData);
extern void _mfcc_preEmphasize(double *sample,int len,double factor);