#pragma once
class WIN
{
public:
	WIN();
	~WIN();

private:
	int m_mixtureNumber; //
	int m_dimensionNumber; //
	double* m_priors; // weight
	double** m_means;
	double** m_variance;
};
