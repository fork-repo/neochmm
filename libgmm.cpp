#include "gmm.h"
#include "KMeans.h"
#include <assert.h>
#include <stdio.h>
#include <Math.h>
#include <string.h>
#include <algorithm> //max()
//#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

GMM::GMM()
{
}
GMM::GMM(int dimensionNumber, int mixtureNumber)
{
	m_dimensionNumber = dimensionNumber;
	m_mixtureNumber = mixtureNumber;
	m_priors = new float[m_mixtureNumber];
	m_means = new float*[m_mixtureNumber];
	m_variances = new float*[m_mixtureNumber];
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		m_means[i] = new float[m_dimensionNumber];
		m_variances[i] = new float[m_dimensionNumber];
	}

	//init
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		m_priors[i] = 1.0/m_mixtureNumber;
		for (int j = 0; j < m_dimensionNumber; j++)
		{
			m_means[i][j] = 0.0;
			m_variances[i][j] = 1.0;
		}
	}
	//m_minVars = new double[m_dimensionNumber];
}

void GMM::Dispose()
{
	delete[] m_priors;

	for (int i = 0; i < m_mixtureNumber; i++)
	{
		delete[] m_means[i];
		delete[] m_variances[i];
	}
	delete[] m_means;
	delete[] m_variances;
}

GMM::~GMM()
{
	Dispose();
}

void GMM::PrintGMM()
{
	
	printf("m_priors:\n\t");
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		printf("%lf ",m_priors[i]);
	}
	printf("\nm_means:\n\t");
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		for (int j = 0; j < m_dimensionNumber; j++)
		{
			printf("%lf ",m_means[i][j]);
		}
		printf("\n\t");
	}
	printf("\nm_variances:\n\t");
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		for (int j = 0; j < m_dimensionNumber; j++)
		{
			printf("%lf ",m_variances[i][j]);
		}
		printf("\n\t");
	}

	printf("\n");

}
//計算v點在Gaussian Mixture Model的機率
float GMM::GetProbability(const float* v)
{
	float prob = 0;
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		//printf("CalNormalProbability([%lf,%lf,%lf], %d) = %lf\n",v[0],v[1],v[2],i,CalNormalProbability(v, i));
		prob += m_priors[i] * CalNormalProbability(v, i);
	}
	return prob;
}

//計算v點在編號為index的Gaussian分佈的機率
float GMM::CalNormalProbability(const float* v, int index)
{
	float prob = 1;
	for (int d = 0; d < m_dimensionNumber; d++)
	{
		prob *= 1 / sqrt(2 * 3.14159 * m_variances[index][d]);
		//printf("m_dimensionNumber=%d,prob=%lf,m=%lf,index=%d\n",d,prob,m_variances[index][d],index);	
		prob *= exp(-0.5 * ((v[d] - m_means[index][d]) * (v[d] - m_means[index][d])) / m_variances[index][d]);
	}
	return prob;
}

void GMM::Train(float *data, int size, float endCondition)
{
	/*
	printf("Start GMM Train\n");
	for(int i = 0; i < size ; i++){
		for(int j = 0; j < m_dimensionNumber ; j++){
			printf("%lf \n",*(data+(i*m_dimensionNumber+j)));
		}
		printf("\n");
	}
	*/	
	//init 初始化m_priors[], m_means[], m_variances[]
	const float MIN_VAR = 1E-10;
	//利用kmans由data求出m_means[]
	KMeans* kmeans = new KMeans(m_dimensionNumber, m_mixtureNumber, 0.01);
	int *Label;
	Label=new int[size];
	//printf("Start KMeans Cluster\n");
	kmeans->Cluster(data,size,Label);
	//
	//printf("Finish KMeans Cluster\n");
	int* cluster_sizes = new int[m_mixtureNumber];
	float* overallMeans = new float[m_dimensionNumber];
	float* overallMinVariances = new float[m_dimensionNumber];
	//更新m_means[]
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		cluster_sizes[i] = 0;
		m_priors[i] = 0;
		memcpy(m_means[i], kmeans->GetMean(i), sizeof(float) * m_dimensionNumber);
		memset(m_variances[i], 0.0, sizeof(float) * m_dimensionNumber);
	}
	memset(overallMeans, 0, sizeof(float) * m_dimensionNumber);
	memset(overallMinVariances, 0, sizeof(float) * m_dimensionNumber);
	//計算m_variances[]
	float *v = new float[m_dimensionNumber];
	int label = -1;
	for (int i = 0; i < size; i++){
		for(int j=0;j<m_dimensionNumber;j++){
			v[j]=data[i*m_dimensionNumber+j];
		}
		//label即為該編號的Gaussian Mixture Model,表示kmeans完後把該筆data分到該GMM裡
		label=Label[i];
		//該GMM數量加1
		cluster_sizes[label]++;
		//取出v[]所屬的群心來計算m_variances[]
		float* tmp_mean = kmeans->GetMean(label);
		for (int j = 0; j < m_dimensionNumber; j++)
		{
			m_variances[label][j] += (v[j] - tmp_mean[j]) * (v[j] - tmp_mean[j]);
		}
		//累加overallMeans[],overallMinVariances[]
		for (int j = 0; j < m_dimensionNumber; j++)
		{
			overallMeans[j] += v[j];
			overallMinVariances[j] += v[j] * v[j];
		}
	}

	//計算overallMeans[],overallMinVariances[]
	for (int j = 0; j < m_dimensionNumber; j++)
	{
		overallMeans[j] /= size;
		//MIN_VAR= 1E-10;
		overallMinVariances[j] = (float) max(MIN_VAR, (float)0.01 * (overallMinVariances[j] / size - overallMeans[j] * overallMeans[j]));
	}

	for (int i = 0; i < m_mixtureNumber; i++)
	{
		m_priors[i] = 1.0 * cluster_sizes[i] / size;
		if (m_priors[i] > 0){
			for (int j = 0; j < m_dimensionNumber; j++)
			{
				//m_variances[]不得為0
				m_variances[i][j] = m_variances[i][j] / cluster_sizes[i];
				if (m_variances[i][j] < overallMinVariances[j])
				{
					m_variances[i][j] = overallMinVariances[j];
				}
			}
			
		}

	}
	delete kmeans;
	delete[] cluster_sizes;
	delete[] Label;
	//init finish

	bool loop = true;
	float lastL = 0;
	float cost = 0, lastCost;
	int unchanged = 0;
	float* new_priors = new float[m_mixtureNumber];
	float** new_variances = new float*[m_mixtureNumber];
	float** new_means = new float*[m_mixtureNumber];

	for (int i = 0; i < m_mixtureNumber; i++)
	{
		new_means[i] = new float[m_dimensionNumber];
		new_variances[i] = new float[m_dimensionNumber];
	}
	//printf("enter GMM train while loop\n");
	while (loop)
	{
		//清除上次計算的值
		memset(new_priors, 0, sizeof(float) * m_mixtureNumber);
		for (int i = 0; i < m_mixtureNumber; i++)
		{
			memset(new_variances[i], 0, sizeof(float) * m_dimensionNumber);
			memset(new_means[i], 0, sizeof(float) * m_dimensionNumber);
		}
		cost = 0;
		//
		for (int k = 0; k < size; k++){
			for(int j=0;j<m_dimensionNumber;j++){
				v[j]=data[k*m_dimensionNumber+j];
			}
			//prob表示資料v在GMM的機率
			float prob = GetProbability(v);
			for (int i = 0; i < m_mixtureNumber; i++)
			{
				//計算該資料v在編號i的gaussian model中的機率,再乘上該gaussian model的priors,最後除上該資料v在整體GMM的機率
				float priors_each_gaussian_model = CalNormalProbability(v, i) * m_priors[i] / prob;
				new_priors[i] += priors_each_gaussian_model;
				for (int j = 0; j < m_dimensionNumber; j++)
				{
					//計算new_means跟new_variances
					new_means[i][j] += priors_each_gaussian_model * v[j];
					new_variances[i][j] += priors_each_gaussian_model * v[j] * v[j];
				}
			}
			//如果prob大於1E-20,currL為log(prob),否則為-20
			cost += (prob > 1E-20) ? log10(prob) : -20;
		}
		cost /= size;
		for (int i = 0; i < m_mixtureNumber; i++)
		{
			//更新m_priors[]
			m_priors[i] = new_priors[i] / size;
			if (m_priors[i] > 0)
			{
				for (int j = 0; j < m_dimensionNumber; j++)
				{
					//不是除以size
					m_means[i][j] = new_means[i][j] / new_priors[i];
					m_variances[i][j] = new_variances[i][j] / new_priors[i] - m_means[i][j] * m_means[i][j];
					if (m_variances[i][j] < overallMinVariances[j])
					{
						m_variances[i][j] = overallMinVariances[j];
					}
				}
			}
		}
		//terminal conditcon
		if (fabs(lastCost - cost) <= endCondition * fabs(lastCost)){
			unchanged++;
			if (unchanged > 3){
				loop = false;
			}
		}
		//printf("GMM(%0.2f,%0.2f,%0.2f[lastCost-cost])\n", lastCost,cost,fabs(lastCost - cost));
		lastCost = cost;
	}
	delete[] overallMeans;
	delete[] overallMinVariances;
	delete[] new_priors;
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		delete[] new_means[i];
		delete[] new_variances[i];
	}
	delete[] new_means;
	delete[] new_variances;
	delete[] v;
}
void GMM::Train(vector<float*> data_sequence, float endCondition)
{
	float* data = new float[data_sequence.size()*m_dimensionNumber];
	for(int i =0; i < data_sequence.size() ; i++){
		memcpy(data+(i*m_dimensionNumber), data_sequence[i],sizeof(float)*m_dimensionNumber);
	}
	/*
	printf("[neo] %d\n", data_sequence.size() );
	for(int i =0; i < data_sequence.size() ; i++){
		printf("[neo]%lf,%lf,%lf\n", data[i*m_dimensionNumber+0],data[i*m_dimensionNumber+1],data[i*m_dimensionNumber+2] );
	}
	*/
	this->Train(data, data_sequence.size(), endCondition);
	delete[] data;
}
void GMM::Train(const char* fileName,float endCondition)
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
	
	float* data = new float[sequence_size*dimensionNumber];
	
	//printf("\ndata:\n");
	for (int i = 0; i < sequence_size; i++){
		//double* v = new double[dimensionNumber];
		sampleFile.read((char*)&data[i*dimensionNumber], sizeof(float) * dimensionNumber);
		//-------------
		//for (int j = 0; j < dimensionNumber; j++){
		//	printf("%lf ",data[i*dimensionNumber+j]);
		//}
		//printf("\n");
		//-------------
	}
	sampleFile.close();
	this->Train(data, sequence_size, endCondition);
}

//定義'<<'運算子 out << gmm
ostream& operator<<(ostream& out, GMM& gmm)
{
	out << "<GMM>" << endl;
	out << "<DimensionNumber> " << gmm.m_dimensionNumber << " </DimensionNumber>" << endl;
	out << "<MixtureNumber> " << gmm.m_mixtureNumber << " </MixtureNumber>" << endl;

	out << "<Prior> ";
	for (int i = 0; i < gmm.m_mixtureNumber; i++){
		out << gmm.m_priors[i] << " ";
	}
	out << "</Prior>" << endl;

	out << "<Mean>" << endl;
	for (int i = 0; i < gmm.m_mixtureNumber; i++){
		for (int j = 0; j < gmm.m_dimensionNumber; j++){
			out << gmm.m_means[i][j] << " ";
		}
		out << endl;
	}
	out << "</Mean>" << endl;

	out << "<Variance>" << endl;
	for (int i = 0; i < gmm.m_mixtureNumber; i++){
		for (int j = 0; j < gmm.m_dimensionNumber; j++){
			out << gmm.m_variances[i][j] << " ";
		}
		out << endl;
	}
	out << "</Variance>" << endl;

	out << "</GMM>" << endl;

	return out;
}

//定義'>>'運算子 out >> gmm
istream& operator>>(istream& in, GMM& gmm)
{
	char label[50];
	in >> label; // "<GMM>"
	assert(strcmp(label, "<GMM>") == 0);

	in >> label >> gmm.m_dimensionNumber >> label; // "<DimNum>"
	in >> label >> gmm.m_mixtureNumber >> label; // "<MixNum>"
	//clean m_priors[], m_means[], m_variances[]
	gmm.Dispose();
	// malloc m_priors[], m_means[], m_variances[]
	gmm.m_priors = new float[gmm.m_mixtureNumber];
	gmm.m_means = new float*[gmm.m_mixtureNumber];
	gmm.m_variances = new float*[gmm.m_mixtureNumber];
	for (int i = 0; i < gmm.m_mixtureNumber; i++)
	{
		gmm.m_means[i] = new float[gmm.m_dimensionNumber];
		gmm.m_variances[i] = new float[gmm.m_dimensionNumber];
	}

	//init
	for (int i = 0; i < gmm.m_mixtureNumber; i++)
	{
		gmm.m_priors[i] = 1.0/gmm.m_mixtureNumber;
		for (int j = 0; j < gmm.m_dimensionNumber; j++)
		{
			gmm.m_means[i][j] = 0.0;
			gmm.m_variances[i][j] = 1.0;
		}
	}

	//

	in >> label; // "<Prior>"
	for (int i = 0; i < gmm.m_mixtureNumber; i++)
	{
		in >> gmm.m_priors[i];
	}
	in >> label; // "</Prior>"

	in >> label; // "<Mean>"
	for (int i = 0; i < gmm.m_mixtureNumber; i++)
	{
		for (int j = 0; j < gmm.m_dimensionNumber; j++)
		{
			in >> gmm.m_means[i][j];
		}
	}
	in >> label; // "</Mean>"

	in >> label; // "<Variance>"
	for (int i = 0; i < gmm.m_mixtureNumber; i++)
	{
		for (int j = 0; j < gmm.m_dimensionNumber; j++)
		{
			in >> gmm.m_variances[i][j];
		}
	}
	in >> label; // "</Variance>"

	in >> label; // "</GMM>"
	return in;
}
