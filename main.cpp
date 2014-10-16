//#pragma once
//
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h> 
#include <windows.h>
#include <mmsystem.h>  
#include <io.h>
#include <string.h>
#include <commctrl.h>        
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

short* audiodate;
double* val;
void draw()
{
  for (int i=0;i< 220500;i++){
    //drawLine(pid,i*(600.0/220500.0), 0+200, i*(600.0/220500.0), (val[i]*100)+200, 1, RGB(255,0,255));
  }
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

int main(int argc, char** argv)
{
  
	double data[] = {
        0.0, 0.2, 0.4,
        0.3, 0.2, 0.4,
        0.4, 0.2, 0.4,
        0.5, 0.2, 0.4,
        5.0, 5.2, 8.4,
        6.0, 5.2, 7.4,
        4.0, 5.2, 4.4,
        10.3, 10.4, 10.5,
        10.1, 10.6, 10.7,
        11.3, 10.2, 10.9
    };
    
    printf("\n---chmm---\n");
    /*
    //GMM的Mixture 數量不得高於train的data數量?
    CHMM *pchmm = new CHMM(5,3,1);
    //pchmm->InitFromModel("chmm.txt",pchmm);
    pchmm->InitFromData("data.seq");
    pchmm->Train("data.seq",0.0000001);
    ofstream chmm_file("chmm.txt");
    assert(chmm_file);
    chmm_file<<*pchmm;
    chmm_file.close();
    */
     CHMM *read_chmm = new CHMM(5,3,1);
     read_chmm->InitFromModel("chmm.txt",read_chmm);
     //read_chmm->PrintModel();
     SAMPLES* psamples = read_chmm->ReadDataBinary("data.seq");
     int i=1;
     int* labels = new int[psamples->sample_size];
     double p = read_chmm->Decode(psamples,labels);
     for(int i=0; i < psamples->sample_size ; i++){
        printf("%d ", labels[i]);
     }
     printf(": prob=%lf\n",p);
     WIN *pw =  new WIN();
     Init();
     pid = CreateWindows("abcdefg",600,600,quit, update);
     button = CreateButton("button",10,500,100, 50, pid, buttonclicked1);
     button2 = CreateButton("button2",200,500,100, 50, pid, buttonclicked2);
     //DisableComponent(button2);
  

    //OutputWAV("neo.wav", 10);
    WAV_HEADER wavHeader;
    WAV_DATA wavData;
    long samplesize=0;
    bool ret;
    ret = ConvertWAVtoSampleData("neo.wav",&val,&samplesize,&wavHeader, &wavData);
    //short *datas;
    //long datasize;
    //WAV_HEADER wavOutHeader;
    //WAV_DATA wavOutData;
    //ReadWAV("neo.wav",&datas, &datasize,&wavOutHeader, &wavOutData);
    //PlayWAV(datas,wavData.Subchunk2Size, 10);
     
    _mfcc_preEmphasize(val,samplesize,0.97);
    //printf("ret=%d\n",ret);
    //
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