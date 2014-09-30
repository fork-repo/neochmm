#pragma once
class GMM
{
public:
	GMM(int, int);
	~GMM();
	void PrintGMM();

private:
	int m_mixtureNumber; //
	int m_dimensionNumber; //
	double* m_priors; // weight
	double** m_means;
	double** m_variance;
};
