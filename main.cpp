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
/*
void render(COMPONENTID* pid) 
{  
    int g_nWidth = 600;
    int g_nHeight = 500;
    //取的hDC
    HDC hDC = GetDC(pid->hwnd);
    //建立暫存DC等等繪圖完拷貝到hDC
    HDC memDC = CreateCompatibleDC(0);
    //指定暫存DC的畫布大小
    HBITMAP bmpBack = CreateCompatibleBitmap(hDC,g_nWidth,g_nHeight);
    SelectObject(memDC,bmpBack);
    //指定暫存DC的畫筆樣式
    HPEN penBack = CreatePen(PS_SOLID,1,RGB(255,0,255));
    SelectObject(memDC,penBack);
    //指定暫存DC的筆刷樣式  
    HBRUSH brushBack = CreateSolidBrush(RGB(255,255,255)); 
    SelectObject(memDC,brushBack);  
    //清除視窗
    //取得視窗尺寸
    RECT rcClient; 
    GetClientRect(pid->hwnd,&rcClient);
    //設定清除視窗顏色為白色
    HBRUSH brushClean = (HBRUSH)GetStockObject(WHITE_BRUSH);
    FillRect(memDC,&rcClient,brushClean);
   
    HBRUSH brushObj = CreateSolidBrush(RGB(0,255,0));
      
    int dw = 30;  
    int rows = g_nHeight/dw;  
    int cols = g_nWidth/dw;  
    for (int r=0; r<rows; ++ r)  
    {  
        for (int c=0; c<cols; ++c)  
        {  
            if (r == c)  
            {  
                SelectObject(memDC,brushObj);  
            }  
            else  
            {  
                SelectObject(memDC,brushBack);  
            }  
            Rectangle(memDC,c*dw,r*dw,(c+1)*dw,(r+1)*dw);  
        }  
    }  
    //拷貝暫存DC到hDC
    BitBlt(hDC,0,0,g_nWidth,g_nHeight,memDC,0,0,SRCCOPY);
    DeleteObject(brushObj);  
    DeleteObject(penBack); 
    DeleteObject(brushBack); 
    DeleteObject(brushClean); 
    DeleteObject(bmpBack);  
    DeleteDC(memDC);         
    ReleaseDC(pid->hwnd,hDC);
   // Sleep(1);  
}  
*/
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