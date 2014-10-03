#pragma once
#include <vector>
#include "gmm.h"
using namespace std;

class SAMPLES
{
public:
	int sample_size;
	int sample_dimension;
	double *data;
};

class CHMM
{
public:
	CHMM();
	CHMM(int stateNumber, int dimensionNumber, int mixtureNumber);
	~CHMM();
	void Train(const char* fileName);
	SAMPLES* ReadDataBinary(const char* fileName);
	void ReadDataBinaryToInitGMMs(const char*);
	friend std::ostream& operator<<(std::ostream& out, CHMM& chmm);
	friend std::istream& operator>>(std::istream& in, CHMM& chmm);

private:
	int m_stateNumber;      //狀態數目
	GMM** m_gmms;    			//Gaussian Mixture Modols
	double* m_pi;   		//
	double** m_A;  			//狀態轉移機率
};