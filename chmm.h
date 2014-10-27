#pragma once
#include <vector>
#include "gmm.h"
using namespace std;

class SAMPLES
{
public:
	int sample_size;
	int sample_dimension;
	float *data;
};

class CHMM
{
public:
	CHMM();
	CHMM(const char* fileName);
	CHMM(int stateNumber, int dimensionNumber, int mixtureNumber);
	~CHMM();
	void Train(const char*, float);
	void Train(SAMPLES* pSamples,float endError);

	SAMPLES* ReadDataBinary(const char*);
	SAMPLES* ConvertFloatDataToSamples(float** data, int sequence_size, int dimensionNumber);
	void ReadDataBinaryToInitGMMs(const char*);
	friend std::ostream& operator<<(std::ostream& out, CHMM& chmm);
	friend std::istream& operator>>(std::istream& in, CHMM& chmm);
	float Decode(SAMPLES* pSamples, int*labels);
	void PrintModel(void);
	void InitFromData(const char* fileName);
	void InitFromData(float** data, int sequence_size, int dimensionNumber);
	void InitFromSamples(SAMPLES* pSamples);
	void InitFromModel(const char* fileName,CHMM* pchmm);
	int GetDimensionNumber(){ return m_dimensionNumber; };
	int GetStateNumber(){ return m_stateNumber; };
	int GetMixtureNumber(){ return m_mixtureNumber; };
	void SaveModel(const char* filename);
private:
	bool isInited;
	long training_sample_size;
	int m_stateNumber;      //狀態數目
	int m_mixtureNumber;
	int m_dimensionNumber; 
	GMM** m_gmms;    			//Gaussian Mixture Modols有m_stateNumber個狀態每個狀態有m_mixtureNumber個
	float* m_pi;   		//有m_stateNumber個
	float** m_A;  			//狀態轉移機率有[m_stateNumber][m_stateNumber+1]個
	float LogProbability(float prob);
};