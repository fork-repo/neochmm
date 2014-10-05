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
    //GMM的Mixture 數量不得高於train的data數量?
    CHMM *pchmm = new CHMM(5,3,1);
    //pchmm->InitFromModel("chmm.txt",pchmm);
    pchmm->InitFromData("data.seq");
    pchmm->Train("data.seq",0.0000001);
    ofstream chmm_file("chmm.txt");
    assert(chmm_file);
    chmm_file<<*pchmm;
    chmm_file.close();
    
     CHMM *read_chmm = new CHMM(5,3,1);
     read_chmm->InitFromModel("chmm.txt",read_chmm);
     //read_chmm->PrintModel();
     SAMPLES* psamples = read_chmm->ReadDataBinary("data.seq");
     int i=1;
     //printf("%lf,%lf,%lf\n",*(psamples->data+(i*3+0)),*(psamples->data+(i*3+1)),*(psamples->data+(i*3+2))) ;
     int* labels = new int[psamples->sample_size];
     double p = read_chmm->Decode(psamples,labels);
     for(int i=0; i < psamples->sample_size ; i++){
        printf("%d ", labels[i]);
     }
     printf("p=%lf\n",p);
     
    /*
    // predict
     CHMM *pchmm = new CHMM(5,3,1);
     pchmm->InitFromModel("chmm.txt",pchmm);
     pchmm->PrintModel();
     SAMPLES* psamples = pchmm->ReadDataBinary("data.seq");
     int* labels = new int[psamples->sample_size];
     pchmm->Decode(psamples,labels);
     for(int i=0; i < psamples->sample_size ; i++){
        printf("%d ", labels[i]);
     }
     printf("\n");
     */
     return 0;
}
