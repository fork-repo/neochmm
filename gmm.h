
#pragma once
#include <fstream> //for std::ostream
class GMM
{
public:
	GMM();
	GMM(int, int);
	~GMM();
	void PrintGMM();
	double GetProbability(const double*);
	void Train(double *, int, double);
	friend std::ostream& operator<<(std::ostream& out, GMM& gmm);
	friend std::istream& operator>>(std::istream& in, GMM& gmm);

private:
	int m_mixtureNumber; //
	int m_dimensionNumber; //
	double* m_priors; // weight
	double** m_means;
	double** m_variances;
	double CalNormalProbability(const double*, int);
	void Dispose();
};
