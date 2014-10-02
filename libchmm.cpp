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
void CHMM::Train(const char* fileName)
{

}
//<sequence_size_M><dimension_N><data_11><data_12>...<data_1N><data_21>...<data_MN>
void CHMM::ReadDataBinaryToInitGMMs(const char* fileName)
{
	ifstream sampleFile(fileName, ios_base::binary);
	assert(sampleFile);
	int sequence_size = 0;
	int dimensionNumber = 0;
	sampleFile.read((char*)&sequence_size, sizeof(int));
	//printf("sequence_size=%d\n",sequence_size);
	//gcount()以2bytes為單位ffff, int以4bytes為單位ffff ffff
	//printf("c=%d\n",sampleFile.gcount());
	sampleFile.read((char*)&dimensionNumber, sizeof(int));
	//printf("dimensionNumber=%d\n",dimensionNumber);
	//有sequence_size筆dimensionNumber維度的資料, m_stateNumber個狀態
	//依序切分(sequence_size/m_stateNumber)個資料給每個狀態
	//data_for_each_state[狀態編號][均分到的個數][資料的維度]
	double** data_for_each_state = new double*[m_stateNumber];
	for (int i = 0; i < m_stateNumber; i++){
		data_for_each_state[i] = new double[(sequence_size/m_stateNumber)*dimensionNumber];
		//for (int j = 0; j < (sequence_size/m_stateNumber); j++){
		//	data_for_each_state[i][j] = new double[dimensionNumber];
		//}
	}

	//printf("\ndata:\n");
	double* v = new double[dimensionNumber];
	for (int i = 0; i < sequence_size; i++){
		sampleFile.read((char*)v, sizeof(double) * dimensionNumber);
		memcpy(data_for_each_state[i/(sequence_size/m_stateNumber)]+(i%(sequence_size/m_stateNumber)), v,sizeof(double) * dimensionNumber);
		//-------------
		//printf("[%d,%d]\n",i/(sequence_size/m_stateNumber),i%(sequence_size/m_stateNumber));
		//for (int j = 0; j < dimensionNumber; j++){
		//	printf("%lf ",data_for_each_state[i/(sequence_size/m_stateNumber)][i%(sequence_size/m_stateNumber)+j]);
		//}
		//printf("\n");
		//-------------
	}
	//用均分的data初始化各個gmms
	for ( int i = 0; i < m_stateNumber; i++)
	{
		m_gmms[i]->Train(data_for_each_state[i],(sequence_size/m_stateNumber),0.00001);
		printf("<m_gmms[%d]>\n",i);
		m_gmms[i]->PrintGMM();
		printf("</m_gmms[%d]>\n",i);
	}
	//clear
	for ( int i = 0; i < m_stateNumber; i++){
		delete[] data_for_each_state[i];
	}
	delete[] v;
}

CHMM::CHMM(int stateNumber, int dimensionNumber, int mixtureNumber)
{
	//初始化 m_C[][], m_pi[], m_A[][]
	m_stateNumber = stateNumber;
	m_gmms = new GMM*[m_stateNumber];
	m_pi = new double[m_stateNumber];
	m_A = new double*[m_stateNumber];
	for (int i = 0; i < m_stateNumber; i++){
		//初始化GMM
		m_gmms[i] = new GMM(dimensionNumber, mixtureNumber);
		m_A[i] = new double[m_stateNumber + 1]; // 加上最後的狀態
	}
	for (int i = 0; i < m_stateNumber; i++){
		// The initial probabilities
		m_pi[i] = 1.0 / m_stateNumber;
		// The transition probabilities
		for (int j = 0; j <= m_stateNumber; j++)
		{
			m_A[i][j] = 1.0 / (m_stateNumber + 1);
		}
	}
	//
}
CHMM::~CHMM()
{
}
