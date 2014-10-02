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

	GMM *pgmm = new GMM(3,5);
	pgmm->Train(data,10,0.00001); //Training GMM
    ofstream gmm_file("gmm.txt");
    assert(gmm_file);
    gmm_file<<*pgmm;
    gmm_file.close();
    delete pgmm;

    GMM *p_readgmm = new GMM();
    ifstream read_gmm_file("gmm.txt");
    assert(read_gmm_file);
    read_gmm_file>>*p_readgmm;
    read_gmm_file.close();
    p_readgmm->PrintGMM();
    printf("\n---chmm---\n");
    CHMM *pchmm = new CHMM(10,3,5);
    pchmm->Init("neo.txt");
    printf("\nend\n");
    return 0;
}
