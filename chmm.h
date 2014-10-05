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
	void Train(const char*, double);
	SAMPLES* ReadDataBinary(const char*);
	void ReadDataBinaryToInitGMMs(const char*);
	friend std::ostream& operator<<(std::ostream& out, CHMM& chmm);
	friend std::istream& operator>>(std::istream& in, CHMM& chmm);
	double Decode(SAMPLES* pSamples, int*labels);
	void PrintModel(void);
private:
	int m_stateNumber;      //狀態數目
	int m_mixtureNumber;
	int m_dimensionNumber; 
	GMM** m_gmms;    			//Gaussian Mixture Modols
	double* m_pi;   		//
	double** m_A;  			//狀態轉移機率
	double LogProbability(double prob);
};