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

void draw()
{
  drawFillRect(pid,0, 0, 600, 500, RGB(0,0,0));
/*
  for (int i=0;i< 512;i++){
    drawLine(pid,i*(600.0/512.0), 0+200, i*(600.0/512.0), (small_data[i]*100)+200, 1, RGB(255,0,255));
  }
  */
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
     for(int i=0; i < testSamples->sample_size ; i++){
        printf("%d ", labels[i]);
     }
     printf(": prob=%lf\n",p);
      for(int i=0;i<sequence_size;i++){
       delete data[i];
    }
    delete data;
}

int main(int argc, char** argv)
{
    char str[256];
    printf("Training:\n----\n");
    //GMM的Mixture 數量不得高於train的data數量?
    int stateNumber = 3;
    int dimensionNumber = 13;
    int mixtureNumber = 4;
    int fborder = 23;
    CHMM *neochmm = init_audio_chmm("data/yes/yes1.wav", stateNumber, dimensionNumber, mixtureNumber, fborder);
    for(int i=1;i<=6;i++){
        sprintf(str, "data/yes/yes%d.wav",i);
        printf("training %s!\n", str);
        train_from_wav(str, neochmm, fborder);
    }
    printf("----\nPredict:\n----\n");
    float prob  =  predit_from_wav("data/yes/yes6.wav", neochmm, fborder);
    printf("----\nSave Model:\n----\n");
    sprintf(str, "chmm.txt");
    printf("save to %s!\n",str);
    neochmm->SaveModel(str);
     //
     WIN *pw =  new WIN();
     Init();
     pid = CreateWindows("abcdefg",600,600,quit, update);
     button = CreateButton("button",10,500,100, 50, pid, buttonclicked1);
     button2 = CreateButton("button2",200,500,100, 50, pid, buttonclicked2);
     //DisableComponent(button2);

    //short *datas;
    //long datasize;
    //WAV_HEADER wavOutHeader;
    //WAV_DATA wavOutData;
    //ReadWAV("neo.wav",&datas, &datasize,&wavOutHeader, &wavOutData);
    //PlayWAV(datas,wavData.Subchunk2Size, 10);
     
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