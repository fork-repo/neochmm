#include "KMeans.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>  
#include <Math.h>
KMeans::KMeans(int dimensionNumber, int clusterNumber, float endCondition)
{
	m_clusterNumber = clusterNumber;
	m_dimensionNumber = dimensionNumber;
	m_endCondition = endCondition;
	m_means = new float*[m_clusterNumber];
	for (int i = 0; i < m_clusterNumber; i++)
	{
		m_means[i] = new float[m_dimensionNumber];
		memset(m_means[i], 0, sizeof(float) * m_dimensionNumber);
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
float KMeans::Distance(const float* v1, const float* v2, int dimensionNumber)
{
	float distance = 0;
	for (int d = 0; d < dimensionNumber; d++)
	{
		distance += (v1[d] - v2[d]) * (v1[d] - v2[d]);
	}
	return sqrt(distance);
}
//return distance
float KMeans::PredictLabel(float* v1, int* ppredict_label)
{
	float distance = -1;
	for (int i = 0; i < m_clusterNumber; i++){
		float temp = Distance(v1, m_means[i], m_dimensionNumber);
		if (temp < distance || distance == -1){
			distance = temp;
			*ppredict_label = i;
		}
	}
	return distance;
}
void KMeans::Cluster(float *data, int size, int *Label)
{
	assert(size >= m_clusterNumber);
	//init m_means[] from data
	float* temp_mean = new float[m_dimensionNumber];
	for (int i = 0; i < m_clusterNumber; i++){
		//select選出index為0, 1*(size / m_clusterNumber), 2*(size / m_clusterNumber)當作初始化群心
		int select = i * size / m_clusterNumber;
			for(int j = 0; j < m_dimensionNumber; j++){
				temp_mean[j] = data[select*m_dimensionNumber+j];
			}
		memcpy(m_means[i], temp_mean, sizeof(float) * m_dimensionNumber);
	}
	delete[] temp_mean;
	// finish init
	bool loop = true;
	float unchanged = 0;
	float* v_data = new float[m_dimensionNumber];
	int v_label = -1;
	float cost, lastCost;
	float** new_means = new float*[m_clusterNumber];
	for (int i = 0; i < m_clusterNumber; i++)
	{
		new_means[i] = new float[m_dimensionNumber];
	}
	int* new_means_counts_each_cluster = new int[m_clusterNumber];
	while (loop){
		cost = 0.0;
		for (int i = 0; i < m_clusterNumber; i++)
		{
			memset(new_means[i], 0, sizeof(float) * m_dimensionNumber);
		}
		memset(new_means_counts_each_cluster, 0, sizeof(int) * m_clusterNumber);
	
		for (int i = 0; i < size; i++){
				for(int j = 0; j < m_dimensionNumber; j++){
					v_data[j] = data[i*m_dimensionNumber+j];
				}
				cost += PredictLabel(v_data, &v_label); 

				for (int j = 0; j < m_dimensionNumber; j++){
					new_means[v_label][j] += v_data[j];
				}
				new_means_counts_each_cluster[v_label]++;
		}
		cost /= size;
		for (int i = 0; i < m_clusterNumber; i++)
		{
			if (new_means_counts_each_cluster[i] > 0)
			{
				for (int j = 0; j < m_dimensionNumber; j++){
					new_means[i][j] /= new_means_counts_each_cluster[i];
				}
				memcpy(m_means[i], new_means[i], sizeof(float) * m_dimensionNumber);	
			}
		}
		//terminal conditcon
		if (fabs(lastCost - cost) <= m_endCondition * lastCost){
			unchanged++;
			if (unchanged > 3){
				loop = false;
			}
		}
		lastCost = cost;
		//printf("KMeans (lastCost - cost)=%lf, %lf\n",lastCost - cost,m_endCondition * lastCost);
	}
	//output Label
	for (int i = 0; i < size; i++)
	{
		for(int j = 0; j < m_dimensionNumber; j++){
			v_data[j] = data[i*m_dimensionNumber+j];
		}
		PredictLabel(v_data, &v_label);
		Label[i] = v_label;
	}
	delete[] new_means_counts_each_cluster;
	delete[] v_data;
	for (int i = 0; i < m_clusterNumber; i++)
	{
		delete[] new_means[i];
	}
	delete[] new_means;
}
void KMeans::PrintMeans()
{
	printf("KMeans:\n");
	for (int i = 0; i < m_clusterNumber; i++){
			printf("(%.2f", m_means[i][0]);
			for (int j = 1; j < m_dimensionNumber; j++){
				printf(",%.2f",m_means[i][j]);
			}	
			printf(")\n");
	}
}
