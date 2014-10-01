#pragma once
class GMM
{
public:
	GMM(int, int);
	~GMM();
	void PrintGMM();
	double GetProbability(const double*);
	void TrainSingleGaussianDistribution(double *, int);

private:
	int m_mixtureNumber; //
	int m_dimensionNumber; //
	double* m_priors; // weight
	double** m_means;
	double** m_variances;
	double CalNormalProbability(const double*, int);
};
