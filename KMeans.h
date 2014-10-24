#pragma once
class KMeans
{
public:
	KMeans(int,int,float);
	~KMeans();
	void Cluster(float*, int, int*);
	float PredictLabel(float* v1, int* ppredict_label);
	void PrintMeans();
	float* GetMean(int index)	{ return m_means[index]; }
private:
	int m_clusterNumber; 
	int m_dimensionNumber; 
	float m_endCondition;
	float **m_means;
	float Distance(const float*, const float*, int);

};