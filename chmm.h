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
	void InitFromData(const char* fileName);
	void InitFromModel(const char* fileName,CHMM* pchmm);
private:
	bool isInited;
	int m_stateNumber;      //狀態數目
	int m_mixtureNumber;
	int m_dimensionNumber; 
	GMM** m_gmms;    			//Gaussian Mixture Modols有m_stateNumber個狀態每個狀態有m_mixtureNumber個
	double* m_pi;   		//有m_stateNumber個
	double** m_A;  			//狀態轉移機率有[m_stateNumber][m_stateNumber+1]個
	double LogProbability(double prob);
};