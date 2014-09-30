#include "KMeans.h"
#include <string.h>
#include <assert.h>  
#include <Math.h>
KMeans::KMeans(int dimensionNumber, int clusterNumber)
{
	m_clusterNumber = clusterNumber;
	m_dimensionNumber = dimensionNumber;
	m_means = new double*[m_clusterNumber];
	for (int i = 0; i < m_clusterNumber; i++)
	{
		m_means[i] = new double[m_dimensionNumber];
		memset(m_means[i], 0, sizeof(double) * m_dimensionNumber);
	}
}
KMeans::~KMeans()
{
	for (int i = 0; i < m_clusterNumber; i++)
	{
		delete[] m_means[i];
	}
	delete[] m_means;
}
double KMeans::Distance(const double* v1, const double* v2, int dimensionNumber)
{
	double distance = 0;
	for (int d = 0; d < dimensionNumber; d++)
	{
		distance += (v1[d] - v2[d]) * (v1[d] - v2[d]);
	}
	return sqrt(distance);
}
void KMeans::Cluster(double *data, int size, int *Label)
{
	assert(size >= m_clusterNumber);
	//init m_means[] from data
	double* sample = new double[m_dimensionNumber];
	for (int i = 0; i < m_clusterNumber; i++){
		//select選出index為0, 1*(size / m_clusterNumber), 2*(size / m_clusterNumber)當作初始化群心
		int select = i * size / m_clusterNumber;
			for(int j = 0; j < m_dimensionNumber; j++){
				sample[j] = data[select*m_dimensionNumber+j];
			}
		memcpy(m_means[i], sample, sizeof(double) * m_dimensionNumber);
	}
	bool loop = true;
	double* v_data = new double[m_dimensionNumber];
	int v_label = -1;
	double cost = 0.0;

	while (loop){
		loop=false;
		for (int i = 0; i < size; i++){
				for(int j = 0; j < m_dimensionNumber; j++){
					v_data[j] = data[i*m_dimensionNumber+j];
				}
				//cost
		}
	}
}