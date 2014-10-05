
#pragma once
#include <fstream> //for std::ostream
#include <vector>
class GMM
{
public:
	GMM();
	GMM(int, int);
	~GMM();
	void PrintGMM();
	double GetProbability(const double*);
	void Train(double *, int, double);
	void Train(std::vector<double*>, double);
	void Train(const char*,double);
	int GetMixtureNumber(){ return m_mixtureNumber; };
	double* GetPriors() { return m_priors;};
	double** GetMeans() { return m_means;};
	double** GetVariances() { return m_variances;};
	void SetPriors(int i, double val) { m_priors[i] = val;};
	void SetMeans(int i, int j, double val) { m_means[i][j] = val;};
	void SetVariances(int i, int j, double val) { m_variances[i][j] = val;};
	
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
