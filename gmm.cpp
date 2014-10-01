#include "gmm.h"
#include "KMeans.h"
#include <stdio.h>
#include <Math.h>
#include <string.h>
#include <algorithm> //max()
using namespace std;

GMM::GMM(int dimensionNumber, int mixtureNumber)
{
	m_dimensionNumber = dimensionNumber;
	m_mixtureNumber = mixtureNumber;
	m_priors = new double[m_mixtureNumber];
	m_means = new double*[m_mixtureNumber];
	m_variances = new double*[m_mixtureNumber];
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		m_means[i] = new double[m_dimensionNumber];
		m_variances[i] = new double[m_dimensionNumber];
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

GMM::~GMM()
{

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
double GMM::GetProbability(const double* v)
{
	double prob = 0;
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		prob += m_priors[i] * CalNormalProbability(v, i);
	}
	return prob;
}

//計算v點在編號為index的Gaussian分佈的機率
double GMM::CalNormalProbability(const double* v, int index)
{
	double prob = 1;
	for (int d = 0; d < m_dimensionNumber; d++)
	{
		prob *= 1 / sqrt(2 * 3.14159 * m_variances[index][d]);
		prob *= exp(-0.5 * ((v[d] - m_means[index][d]) * (v[d] - m_means[index][d])) / m_variances[index][d]);
	}
	return prob;
}

void GMM::TrainSingleGaussianDistribution(double *data, int size)
{
	//init 初始化m_priors[], m_means[], m_variances[]
	const double MIN_VAR = 1E-10;
	//利用kmans由data求出m_means[]
	KMeans* kmeans = new KMeans(m_dimensionNumber, m_mixtureNumber, 0.01);
	int *Label;
	Label=new int[size];
	kmeans->Cluster(data,size,Label);
	//
	int* cluster_sizes = new int[m_mixtureNumber];
	double* overallMeans = new double[m_dimensionNumber];
	double* overallMinVariances = new double[m_dimensionNumber];
	//更新m_means[]
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		cluster_sizes[i] = 0;
		m_priors[i] = 0;
		memcpy(m_means[i], kmeans->GetMean(i), sizeof(double) * m_dimensionNumber);
		memset(m_variances[i], 0.0, sizeof(double) * m_dimensionNumber);
	}
	memset(overallMeans, 0, sizeof(double) * m_dimensionNumber);
	memset(overallMinVariances, 0, sizeof(double) * m_dimensionNumber);
	//計算m_variances[]
	double *v = new double[m_dimensionNumber];
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
		double* tmp_mean = kmeans->GetMean(label);
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
		overallMinVariances[j] = max(MIN_VAR, 0.01 * (overallMinVariances[j] / size - overallMeans[j] * overallMeans[j]));
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
	double lastL = 0;
	double currL = 0;
	int unchanged = 0;
	double* new_priors = new double[m_mixtureNumber];
	double** new_variances = new double*[m_mixtureNumber];
	double** new_means = new double*[m_mixtureNumber];

	for (int i = 0; i < m_mixtureNumber; i++)
	{
		new_means[i] = new double[m_dimensionNumber];
		new_variances[i] = new double[m_dimensionNumber];
	}

	while (loop)
	{
		loop=false;
		//清除上次計算的值
		memset(new_priors, 0, sizeof(double) * m_mixtureNumber);
		for (int i = 0; i < m_mixtureNumber; i++)
		{
			memset(new_variances[i], 0, sizeof(double) * m_dimensionNumber);
			memset(new_means[i], 0, sizeof(double) * m_dimensionNumber);
		}

		lastL = currL;
		currL = 0;
		//
		for (int k = 0; k < size; k++){
			for(int j=0;j<m_dimensionNumber;j++){
				v[j]=data[k*m_dimensionNumber+j];
			}
			double prob = GetProbability(v);
			printf("%lf\n",prob );
		}
	}
}