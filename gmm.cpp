#include "gmm.h"
#include <stdio.h>
using namespace std;

GMM::GMM(int dimensionNumber, int mixtureNumber)
{
	m_dimensionNumber = dimensionNumber;
	m_mixtureNumber = mixtureNumber;
	m_priors = new double[m_mixtureNumber];
	m_means = new double*[m_mixtureNumber];
	m_variance = new double*[m_mixtureNumber];
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		m_means[i] = new double[m_dimensionNumber];
		m_variance[i] = new double[m_dimensionNumber];
	}

	//init
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		m_priors[i] = 1.0/m_mixtureNumber;
		for (int j = 0; j < m_dimensionNumber; j++)
		{
			m_means[i][j] = 0.0;
			m_variance[i][j] = 1.0;
		}
	}
	//m_minVars = new double[m_dimensionNumber];
}

GMM::~GMM()
{

}

void GMM::PrintGMM()
{
	
	printf("m_priors:\n\t");
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		printf("%lf ",m_priors[i]);
	}
	printf("\nm_means:\n\t");
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		for (int j = 0; j < m_dimensionNumber; j++)
		{
			printf("%lf ",m_means[i][j]);
		}
		printf("\n\t");
	}
	printf("\nm_variance:\n\t");
	for (int i = 0; i < m_mixtureNumber; i++)
	{
		for (int j = 0; j < m_dimensionNumber; j++)
		{
			printf("%lf ",m_variance[i][j]);
		}
		printf("\n\t");
	}

	printf("\n");

}