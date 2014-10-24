
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
	float GetProbability(const float*);
	void Train(float *, int, float);
	void Train(std::vector<float*>, float);
	void Train(const char*,float);
	int GetMixtureNumber(){ return m_mixtureNumber; };
	float* GetPriors() { return m_priors;};
	float** GetMeans() { return m_means;};
	float** GetVariances() { return m_variances;};
	void SetPriors(int i, float val) { m_priors[i] = val;};
	void SetMeans(int i, int j, float val) { m_means[i][j] = val;};
	void SetVariances(int i, int j, float val) { m_variances[i][j] = val;};
	
	friend std::ostream& operator<<(std::ostream& out, GMM& gmm);
	friend std::istream& operator>>(std::istream& in, GMM& gmm);


private:
	int m_mixtureNumber; //
	int m_dimensionNumber; //
	float* m_priors; // weight
	float** m_means;
	float** m_variances;
	float CalNormalProbability(const float*, int);
	void Dispose();
};
