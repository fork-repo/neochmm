//#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h> 
#include <windows.h>    //for win
//#include <fstream>
//#include <iostream>
//#include <mmsystem.h>  
//#include <io.h>
//#include <string.h>
//#include <commctrl.h>        
#include "gmm.h"
#include "win.h"
#include "kmeans.h"
#include "chmm.h"
#include "audiotool.h"

using namespace std;

COMPONENTID* pid;
COMPONENTID* button;
COMPONENTID* button2;
void eventLoop();
void quit()
{
  DeleteDC(pid->memDC);         
  ReleaseDC(pid->hwnd,pid->hDC);
  exit(0);
}

void update()
{

}


short *datas;
long datasize;

void draw()
{
  drawFillRect(pid,0, 0, 600, 500, RGB(0,0,0));

  for (int i=0;i< datasize;i++){
    drawLine(pid,i*(600.0/(datasize)), 0+200, i*(600.0/(datasize)), (datas[i]/32768.0*100)+200, 1, RGB(255,0,255));
  }
  
  // for (int i=0;i< val_size;i++){
   // drawLine(pid,i*(600.0/val_size), 0+200, i*(600.0/val_size), (val[i]*100.0/32768.0)+200, 1, RGB(255,0,255));
  //}
 
  Render(pid, 0 , 0 , 600, 500);
}

void buttonclicked1()
{
  printf("bn1\n");
}
void buttonclicked2()
{
  printf("bn2\n");
}


CHMM* init_audio_chmm(const char* filename,int stateNumber,int dimensionNumber,int mixtureNumber, int fborder)
{
    CHMM *chmm = new CHMM(stateNumber,dimensionNumber,mixtureNumber);
    float** data;
    int sequence_size = readMMFCfromWAV(filename,&data,(dimensionNumber-1), fborder);
    SAMPLES* ps = chmm->ConvertFloatDataToSamples(data, sequence_size, dimensionNumber);
    chmm->InitFromSamples(ps);
    for(int i=0;i<sequence_size;i++){
       delete data[i];
    }
    delete data;
    return chmm;
}

void train_from_wav(const char* filename, CHMM *pchmm, int fborder)
{
    float** data;
    int sequence_size = readMMFCfromWAV(filename,&data,(pchmm->GetDimensionNumber()-1), fborder);
    SAMPLES* ps = pchmm->ConvertFloatDataToSamples(data, sequence_size, pchmm->GetDimensionNumber());
    pchmm->Train(ps,0.0000001);
    for(int i=0;i<sequence_size;i++){
       delete data[i];
    }
    delete data;
}

float predit_from_wav(const char* filename, CHMM *pchmm, int fborder)
{
    float** data;
    int sequence_size = readMMFCfromWAV(filename,&data,(pchmm->GetDimensionNumber()-1), fborder);
    SAMPLES* testSamples = pchmm->ConvertFloatDataToSamples(data, sequence_size, pchmm->GetDimensionNumber());
     int* labels = new int[testSamples->sample_size];
     double p = pchmm->Decode(testSamples,labels);
    /*
     for(int i=0; i < testSamples->sample_size ; i++){
        printf("%d ", labels[i]);
     }
     printf(": prob=%lf\n",p);
     */
      for(int i=0;i<sequence_size;i++){
       delete data[i];
    }
    delete data;
    return p;
}

int main(int argc, char** argv)
{
     int fborder = 23;
   
    
    char str[256];
    /*
    printf("Training:\n----\n");
    //GMM的Mixture 數量不得高於train的data數量?
    CHMM *neochmm = new CHMM(3,13,4);
    float** data;
    int sequence_size = readMMFCfromWAV("data/train/yes/yes0.wav",&data,(neochmm->GetDimensionNumber()-1), fborder);
    SAMPLES* ps = neochmm->ConvertFloatDataToSamples(data, sequence_size, neochmm->GetDimensionNumber());
    neochmm->InitFromSamples(ps);
    neochmm->Train(ps,0.1);

    for(int i=0;i<=399;i++){
        sprintf(str, "data/train/yes/yes%d.wav",i);
        printf("training %s!\n", str);
        sequence_size = readMMFCfromWAV(str,&data,(neochmm->GetDimensionNumber()-1), fborder);
        ps = neochmm->ConvertFloatDataToSamples(data, sequence_size, neochmm->GetDimensionNumber());
        neochmm->Train(ps,0.01);
    }
    printf("----\nSave Model:\n----\n");
    sprintf(str, "chmm.txt");
    printf("save to %s!\n",str);
    neochmm->SaveModel(str);
    */

    CHMM *read_chmm = new CHMM("chmm.txt");
   // read_chmm->PrintModel();
    
    printf("----\nPredict:\n----\n");
    WAV_HEADER wavOutHeader;
    WAV_DATA wavOutData;
    for(int i=1;i<=6;i++){
   
     sprintf(str, "data/yes/yes%d.wav",i);
        ReadWAV(str,&datas, &datasize,&wavOutHeader, &wavOutData);
    //PlayWAV(datas,44100,1500);
        float prob  =  predit_from_wav(str, read_chmm, fborder);
        printf("prob=%lf\n",prob);
    }
    /*
    for(int i=0;i<=399;i++){
        sprintf(str, "data/train/yes/yes%d.wav",100);
        ReadWAV(str,&datas, &datasize,&wavOutHeader, &wavOutData);
    //PlayWAV(datas,44100,1500);
        float prob  =  predit_from_wav(str, read_chmm, fborder);
        printf("prob=%lf\n",prob);
    }
    for(int i=0;i<=399;i++){
        sprintf(str, "data/train/no/no%d.wav",0);
        ReadWAV(str,&datas, &datasize,&wavOutHeader, &wavOutData);
        //PlayWAV(datas,44100,1500);
        float prob2  =  predit_from_wav(str, read_chmm, fborder);
        printf("prob2=%lf\n",prob2);
    }
    */
     //
     WIN *pw =  new WIN();
     Init();
     pid = CreateWindows("abcdefg",600,600,quit, update);
     button = CreateButton("button",10,500,100, 50, pid, buttonclicked1);
     button2 = CreateButton("button2",200,500,100, 50, pid, buttonclicked2);
     //DisableComponent(button2);

     ShowWindows(pid);
     eventLoop();
     return 0;
}

void eventLoop()
{
 int quit=0;
     EVENTS* pEvents = (EVENTS*)malloc(sizeof(EVENTS));
     int eventGet = 0; 
     while(!quit){
        draw();
        //Sleep(2000); 
        eventGet = PullEvents(pEvents); 
        if (eventGet){
            switch (pEvents->eventtype)
            {
                case KEVENT:
                    switch (pEvents->keyevent.key)
                    {
                        case 27:
                            exit(0);
                            break;
                        case 13:
                            printf("press enter\n");                           
                            break;
                        default:
                            printf("key(%u,ctrl[%d],shift[%d])\n",pEvents->keyevent.key,pEvents->keyevent.ctrl,pEvents->keyevent.shift);
                            break;
                    }
                    break;
                case MEVENT:
                    printf("mouse(%d,%d,[%d])\n",pEvents->mouseevent.x,pEvents->mouseevent.y,pEvents->mouseevent.state);
                    break;
                case WEVENT:
                    break;
                default:
                    break;
            }
        }
     }
}