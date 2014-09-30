#pragma once
class KMeans
{
public:
	KMeans(int,int);
	~KMeans();
	void Cluster(double*, int, int*);
private:
	int m_clusterNumber; 
	int m_dimensionNumber; 
	double **m_means;
	double Distance(const double*, const double*, int);

};