#include "chmm.h"
#include "gmm.h"
#include <assert.h>
#include <string.h>
#include <fstream>
//#include <iostream>
//#include <fstream>
//#include <strstream>
//#include <algorithm>
//#include <math.h>
using namespace std;

istream& operator>>(istream& in, CHMM& chmm)
{

}

void CHMM::Init(const char* fileName)
{
	ifstream sampleFile(fileName, ios_base::binary);
	assert(sampleFile);
	int size = 0;
	int dim = 0;
	sampleFile.read((char*)&size, sizeof(int));
	printf("size=%d\n",size);
	printf("c=%d\n",sampleFile.gcount());

	sampleFile.read((char*)&dim, sizeof(int));
	printf("dim=%d\n",dim);
}

CHMM::CHMM(int stateNumber, int dimensionNumber, int mixtureNumber)
{
	//初始化 m_C[][], m_pi[], m_A[][]
	m_C = new GMM*[stateNumber];
	m_pi = new double[stateNumber];
	m_A = new double*[stateNumber];
	for (int i = 0; i < stateNumber; i++){
		m_C[i] = new GMM(dimensionNumber, mixtureNumber);
		m_A[i] = new double[stateNumber + 1]; // 加上最後的狀態
	}
	for (int i = 0; i < stateNumber; i++){
		// The initial probabilities
		m_pi[i] = 1.0 / stateNumber;
		// The transition probabilities
		for (int j = 0; j <= stateNumber; j++)
		{
			m_A[i][j] = 1.0 / (stateNumber + 1);
		}
	}
	//
}
CHMM::~CHMM()
{
}
