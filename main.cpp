//#pragma once
//
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "gmm.h"
#include "win.h"
#include "kmeans.h"
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
    int* labels = new int[10];
    //WIN *pwin = new WIN();
    KMeans *pkmeans = new KMeans(3,5,0.01);
    pkmeans->Cluster(data,10,labels);
    pkmeans->PrintMeans();
    //
    printf("label:\n\t");
    for (int i=0;i<10;i++){
          printf("%d ",labels[i]);
    }
	//GMM *pgmm = new GMM(3,2);
	//pgmm->PrintGMM();
	printf("\nend\n");
}
