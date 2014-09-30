#pragma once
class KMeans
{
public:
	KMeans(int,int,double);
	~KMeans();
	void Cluster(double*, int, int*);
	double PredictLabel(double* v1, int* ppredict_label);
	void PrintMeans();
private:
	int m_clusterNumber; 
	int m_dimensionNumber; 
	double m_endCondition;
	double **m_means;
	double Distance(const double*, const double*, int);

};