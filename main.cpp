//#pragma once
//
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>  
#include "gmm.h"
#include "win.h"
#include "kmeans.h"
#include "chmm.h"

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
  drawFillRect(pid,0, 0, 600, 500, RGB(255,255,255));
  drawLine(pid,0, 0, 100, 100, 3, RGB(255,0,0));
  drawRectangle(pid,00, 50, 450, 400, 30, RGB(0,0,255),RGB(0,255,255));
  drawLine(pid,100, 100, 200, 100, 6, RGB(0,255,0));
  drawLine(pid,400, 200, 0, 50, 1, RGB(255,0,255));
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
    //
    HWAVEIN microHandle;
    WAVEHDR waveHeader;
    const int NUMPTS = 22050 * 10;   // 10 seconds
    int sampleRate = 22050;
    short int waveIn[NUMPTS];   // 'short int' is a 16-bit type; I request 16-bit samples below
                                // for 8-bit capture, you'd use 'unsigned char' or 'BYTE' 8-bit types

    MMRESULT result = 0;
    //宣告format為WAVEFORMATEX格式
    WAVEFORMATEX format;
    //WAVE_FORMAT_PCM表示沒壓縮
    format.wFormatTag=WAVE_FORMAT_PCM;      // simple, uncompressed format
    //8跟16可選
    format.wBitsPerSample=8;                //  16 for high quality, 8 for telephone-grade
    //1跟2表示mono跟stereo
    format.nChannels=1;                     //  1=mono, 2=stereo
    //給22050
    format.nSamplesPerSec=sampleRate;       //  22050
    //
    format.nAvgBytesPerSec=format.nSamplesPerSec*format.nChannels*format.wBitsPerSample/8;
    //                                        // = nSamplesPerSec * n.Channels * wBitsPerSample/8
    format.nBlockAlign=format.nChannels*format.wBitsPerSample/8;                    
     //                                       // = n.Channels * wBitsPerSample/8
    format.cbSize=0;
    //填microHandle
    result = waveInOpen(&microHandle, WAVE_MAPPER, &format, 0L, 0L, WAVE_FORMAT_DIRECT);
    if (result)
    {
       printf("fail\n");
        Sleep(10000);
        return 0; 
    }
    //
     // Set up and prepare header for input
    waveHeader.lpData = (LPSTR)waveIn;
    waveHeader.dwBufferLength = NUMPTS*2;
    waveHeader.dwBytesRecorded=0;
    waveHeader.dwUser = 0L;
    waveHeader.dwFlags = 0L;
    waveHeader.dwLoops = 0L;
    //填waveHeader
    waveInPrepareHeader(microHandle, &waveHeader, sizeof(WAVEHDR));
    
    result = waveInAddBuffer(microHandle, &waveHeader, sizeof(WAVEHDR));

    if (result)
    {
        cout << "Fail step 2" << endl;
        cout << result << endl;
        Sleep(10000);
        return 0;
    }

    result = waveInStart(microHandle);

    if (result)
    {
        cout << "Fail step 3" << endl;
        cout << result << endl;
        Sleep(10000);
        return 0;
    }

     // Wait until finished recording
     do {} while (waveInUnprepareHeader(microHandle, &waveHeader, sizeof(WAVEHDR))==WAVERR_STILLPLAYING);

     waveInClose(microHandle);

    //
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