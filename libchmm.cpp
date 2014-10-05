#include "chmm.h"
#include "gmm.h"
#include <assert.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <iomanip> // for print format
using namespace std;
void CHMM::InitFromData(const char* fileName)
{
	ReadDataBinaryToInitGMMs(fileName);	
	isInited = true;
}
void CHMM::InitFromModel(const char* fileName, CHMM* pchmm)
{
    ifstream read_chmm_file(fileName);
    assert(read_chmm_file);
    read_chmm_file>>*pchmm;
    read_chmm_file.close();
  	isInited = true;
}
void CHMM::Train(const char* fileName, double endError)
{
	assert(isInited);
	bool loop = true;
	int unchanged = 0;
	double cost = 0;
	double lastCost = 0;
	//new_pi,new_A,train_C_data_for_each_state[]用來update
	double *new_pi = new double[m_stateNumber];
	double **new_A = new double*[m_stateNumber];
	for (int i = 0; i < m_stateNumber; i++){
		new_A[i] = new double[m_stateNumber + 1]; // 加上最後的狀態
	}
	SAMPLES* pSamples = ReadDataBinary(fileName);
	int* labels_for_decode = new int[pSamples->sample_size];
	vector<double*> *train_gmm_samples_for_each_state = new vector<double*>[m_stateNumber];
	while (loop)
	{
		lastCost = cost;
		cost = 0;
		//清除所有計算值
		for (int i = 0; i < m_stateNumber; i++)
		{
			for (int j=0;j < train_gmm_samples_for_each_state[i].size();j++){
				train_gmm_samples_for_each_state[i].clear();
			}
			memset(new_A[i], 0, sizeof(double) * (m_stateNumber + 1));
		}
		memset(new_pi, 0, sizeof(double) * m_stateNumber);
		//
		//for (int z = 0; z < 10; z++)
		//{
			//Decode算出該HMM的成本
			cost += (Decode(pSamples, labels_for_decode));
			//cost += LogProbability(Decode(pSamples, labels_for_decode));
			for (int t = 0; t < pSamples->sample_size ;t++){
	 			printf("%d ", labels_for_decode[t]);
			}
			printf("\n");
			//printf("\nc=%lf\n",cost);
			// separate samples into train_gmm_samples_for_each_state[]
			for (int i = 0; i < pSamples->sample_size; i++){
				train_gmm_samples_for_each_state[labels_for_decode[i]].push_back(pSamples->data+(i*pSamples->sample_dimension));
			}
			/*
			printf("\n");
			for (int i = 0; i < m_stateNumber; i++){
				for (int j=0;j < train_gmm_samples_for_each_state[i].size();j++){
					double *v = train_gmm_samples_for_each_state[i][j];
					printf("[%d][%d]%lf,%lf,%lf\n",i,j,v[0],v[1],v[2]);
				}
			}
			*/
			memcpy(new_pi,m_pi,sizeof(double) * m_stateNumber);
			new_pi[labels_for_decode[0]]++;
			for (int i = 0; i < m_stateNumber; i++){
				memcpy(new_A[i],m_A[i],sizeof(double) * (m_stateNumber + 1));	
			}
			for (int j = 0; j < pSamples->sample_size; j++)
			{
				if (j > 0){
					new_A[labels_for_decode[j-1]][labels_for_decode[j]]++;
				}
			}
			new_A[labels_for_decode[pSamples->sample_size-1]][m_stateNumber]++; //final state ++
		//}
		//cost /= 10;
		//更新C
		for (int i = 0; i < m_stateNumber; i++)
		{
			if (train_gmm_samples_for_each_state[i].size() >= m_gmms[i]->GetMixtureNumber() * 2){
				m_gmms[i]->Train(train_gmm_samples_for_each_state[i],0.00001);
			}else{
				//printf("train_gmm_samples_for_each_state[%d].sample_size = %d too small, don't update C\n",i,train_gmm_samples_for_each_state[i].size());
			}
		}
		double denominator=0;
		//計算new_pi[]的總和
		for (int i = 0; i < m_stateNumber; i++){
			denominator += new_pi[i];
		}
		//printf("denominator = %lf\n",denominator);
		//
		//更新pi
		for (int i = 0; i < m_stateNumber; i++){
			m_pi[i] = 1.0 * new_pi[i] / denominator;
		}
		/*
		printf("m_pi[]:\n");
		for (int i = 0; i < m_stateNumber; i++){
			printf("%lf ", m_pi[i]);
		}
		printf("\n");
		*/
		//
		//更新A
		for (int i = 0; i < m_stateNumber; i++){
			denominator = 0;
			// include final state
			//計算new_A[i][]的總和
			for (int j = 0; j < m_stateNumber + 1; j++){
				denominator += new_A[i][j];
			}
			if (denominator > 0){
				for (int j = 0; j < m_stateNumber + 1; j++){
					m_A[i][j] = 1.0 * new_A[i][j] / denominator;
				}
			}
		}	
		//終止條件
		printf("CHMM cost-lastCost(%lf,%lf,%lf)\n",cost,lastCost,endError * fabs(lastCost));
		unchanged = (cost - lastCost <= endError * fabs(lastCost)) ? (unchanged + 1) : 0;
		if ( unchanged >= 3){
			loop = false;
		}
	}
	//
	printf("finish train, decode again:");
	 int* plabels = new int[pSamples->sample_size];
     double p = Decode(pSamples,plabels);
     for(int i=0; i < pSamples->sample_size ; i++){
        printf("%d ", plabels[i]);
     }
     printf(": p=%lf\n",p);
     //
	//PrintModel();
}
void CHMM::PrintModel()
{
	printf("m_stateNumber:\n");
	printf("%d\n",m_stateNumber);
	printf("m_dimensionNumber:\n");
	printf("%d\n",m_dimensionNumber);
	printf("m_mixtureNumber:\n");
	printf("%d\n",m_mixtureNumber);
	printf("m_pi:\n");
	for (int i = 0; i < m_stateNumber; i++){
		printf("%lf ",m_pi[i]);
	}
	printf("\nm_A:\n");
	for (int i = 0; i < m_stateNumber; i++){
		for (int j = 0; j < m_stateNumber+1; j++){
			printf("%lf ",m_A[i][j]);
		}
		printf("\n");
	}

	for (int i = 0; i < m_stateNumber; i++){
		printf("m_gmms[%d]:\n",i);
		printf("m_priors:\n");
		double *m_priors = m_gmms[i]->GetPriors();
		for(int j = 0; j < m_mixtureNumber; j++){
			printf("%lf ",m_priors[j]);
		}
		printf("\n");
		printf("m_means:\n");
		double **m_means = m_gmms[i]->GetMeans();
		for(int j = 0; j < m_mixtureNumber; j++){
			for(int k = 0; k < m_dimensionNumber; k++){
				printf("%lf ",m_means[j][k]);
			}
			printf("\n");
		}
		printf("m_variances:\n");
		double **m_variances = m_gmms[i]->GetVariances();
		for (int j = 0; j < m_mixtureNumber; j++){
			for (int k = 0; k < m_dimensionNumber; k++){
				printf("%lf ",m_variances[j][k]);
			}
			printf("\n");
		}
	}
}

ostream& operator<<(ostream& out, CHMM& chmm)
{
	out << "<CHMM>" << endl;
	out << "<StateNumber> " << chmm.m_stateNumber << " </StateNumber>" << endl;
	out << "<DimensionNumber> " << chmm.m_dimensionNumber << " </DimensionNumber>" << endl;
	out << "<MixtureNumber> " << chmm.m_mixtureNumber << " </MixtureNumber>" << endl;
	out << "<Pi> ";
	for (int i = 0; i < chmm.m_stateNumber; i++){
		out << setiosflags(ios::fixed) << setprecision(6) << chmm.m_pi[i] << " ";
	}
	out << "</Pi>" << endl;
	out << "<A> " << endl;
	for (int i = 0; i < chmm.m_stateNumber; i++){
		for (int j = 0; j < chmm.m_stateNumber+1; j++){
			out  << chmm.m_A[i][j] << " ";
		}
		out << endl;
	}
	out << "</A>" << endl;

	for (int i = 0; i < chmm.m_stateNumber; i++){
		out << "<GMM"<< i << "> " << endl;
		out << "<PRIORS> " << endl;
		double *m_priors = chmm.m_gmms[i]->GetPriors();
		for(int j = 0; j < chmm.m_mixtureNumber; j++){
			out  << m_priors[j] << " ";
		}
		out << endl;
		out << "</PRIORS>" << endl;
		out << "<MEANS> " << endl;
		double **m_means = chmm.m_gmms[i]->GetMeans();
		for(int j = 0; j < chmm.m_mixtureNumber; j++){
			for(int k = 0; k < chmm.m_dimensionNumber; k++){
				out  << m_means[j][k] << " ";
			}
			out << endl;
		}
		out << "</MEANS> " << endl;
		out << "<VARIANCES> " << endl;
		double **m_variances = chmm.m_gmms[i]->GetVariances();
		for(int j = 0; j < chmm.m_mixtureNumber; j++){
			for(int k = 0; k < chmm.m_dimensionNumber; k++){
				out  << m_variances[j][k] << " ";
			}
			out << endl;
		}
		out << "</VARIANCES> " << endl;
		out << "</GMM"<< i << "> " << endl;		
	}
	out << "</CHMM>" << endl;
	return out;
}

//定義'>>'運算子 out >> gmm
istream& operator>>(istream& in, CHMM& chmm)
{
	char label[50];
	in >> label; // "<CHMM>"
	assert(strcmp(label, "<CHMM>") == 0);
	
	in >> label >> chmm.m_stateNumber >> label; // "<StateNumber>"
	in >> label >> chmm.m_dimensionNumber >> label; // "<DimensionNumber>"
	in >> label >> chmm.m_mixtureNumber >> label; // "<MixtureNumber>"
	chmm.m_pi = new double[chmm.m_stateNumber];
	chmm.m_A = new double*[chmm.m_stateNumber];
	chmm.m_gmms = new GMM*[chmm.m_stateNumber];
	for (int i = 0; i < chmm.m_stateNumber; i++){
		chmm.m_A[i] = new double[chmm.m_stateNumber+1];
		chmm.m_gmms[i] = new GMM(chmm.m_dimensionNumber, chmm.m_mixtureNumber);
	}
	in >> label; //<Pi>
	for (int i = 0; i < chmm.m_stateNumber; i++){
		in >> chmm.m_pi[i] ;
	}
	in >> label; //</Pi>
	in >> label; //<A>
	for (int i = 0; i < chmm.m_stateNumber; i++){
		for (int j = 0; j < chmm.m_stateNumber+1; j++){
			in  >> chmm.m_A[i][j];
		}
	}
	in >> label; //</A>
	for (int i = 0; i < chmm.m_stateNumber; i++){
		in >> label; //<GMMi>
		in >> label; //<PRIORS>
		for(int j = 0; j < chmm.m_mixtureNumber; j++){
			double val;
			in >> val;
			chmm.m_gmms[i]->SetPriors(j, val);
		}
		in >> label; //</PRIORS>
		in >> label; //<MEANS>
		for(int j = 0; j < chmm.m_mixtureNumber; j++){
			for(int k = 0; k < chmm.m_dimensionNumber; k++){
				double val;
				in >> val;
				chmm.m_gmms[i]->SetMeans( j, k, val);
			}
		}
		in >> label; //</MEANS>
		in >> label; //<VARIANCES>
		for(int j = 0; j < chmm.m_mixtureNumber; j++){
			for(int k = 0; k < chmm.m_dimensionNumber; k++){
				double val;
				in >> val;
				chmm.m_gmms[i]->SetVariances( j, k, val);
			}
		}
		in >> label; //</VARIANCES>
		in >> label; //</GMMi>
	}
	return in;
}

double CHMM::LogProbability(double prob)
{
	return (prob > 1e-20) ? log10(prob) : -20;
}
//ψ[t][j] 表示第t次狀態為Sj時,t-1次的最可能隱藏狀態為ψ[t][j]
double CHMM::Decode(SAMPLES* pSamples, int*labels)
{
	double* delta = new double[m_stateNumber];
	double* previous_delta = new double[m_stateNumber];
	int** psi = new int*[pSamples->sample_size];
	// Init
	psi[0] = new int[m_stateNumber];
	printf("previous_delta =\n");
	for (int i = 0; i < m_stateNumber; i++){
		// pi * C
		previous_delta[i] = LogProbability(m_pi[i]) + LogProbability(m_gmms[i]->GetProbability(pSamples->data));
		printf("p = %lf\n ", LogProbability(m_gmms[i]->GetProbability(pSamples->data)));
		psi[0][i] = -1;
	}
	printf("\n");
	for (int t = 1; t < pSamples->sample_size; t++){
		psi[t] = new int[m_stateNumber];
		for (int i = 0; i < m_stateNumber; i++)
		{
			// ???? currLogP[i] = -1e308;
			double maxLogProbability = previous_delta[0] + LogProbability(m_A[0][i]);
			psi[t][i] = 0;
			for (int j = 0; j < m_stateNumber; j++)
			{
				//from j to i
				double temp = previous_delta[j] + LogProbability(m_A[j][i]);
				if (temp > maxLogProbability)
				{
					maxLogProbability = temp;
					psi[t][i] = j;
				}
			}
			delta[i] = maxLogProbability + LogProbability(m_gmms[i]->GetProbability(pSamples->data+(t*pSamples->sample_dimension)));
			previous_delta[i] = delta[i];
			//printf("c[%d]=%lf\n",i,delta[i] );
		}
	}
	//計算最後一次的機率即為該seq的decode機率
	int finalState = 0;
	double prob = -1e308;
	for (int i = 0; i < m_stateNumber; i++)
	{
		if (delta[i] > prob)
		{
			prob = delta[i];
			finalState = i;
		}
	}
	//求出labels
	////ψt(j) 表示第t次狀態為 Sj時,t-1 次的最可能隱藏狀態為ψ t(j)
	labels[pSamples->sample_size - 1] = finalState;
	for (int t = pSamples->sample_size - 2 ; t >= 0 ; t--){
	 	labels[t] = psi[t+1][labels[t+1]];
	}
	return prob;
}
//<sequence_size_M><dimension_N><data_11><data_12>...<data_1N><data_21>...<data_MN>
SAMPLES* CHMM::ReadDataBinary(const char* fileName)
{
	SAMPLES* pSamples = new SAMPLES();
	ifstream sampleFile(fileName, ios_base::binary);
	assert(sampleFile);
	int sequence_size = 0;
	int dimensionNumber = 0;
	sampleFile.read((char*)&sequence_size, sizeof(int));
	sampleFile.read((char*)&dimensionNumber, sizeof(int));
	pSamples->data = new double[sequence_size*dimensionNumber];
	pSamples->sample_size = sequence_size;
	pSamples->sample_dimension = dimensionNumber;
	double* v = new double[dimensionNumber];
	for (int i = 0; i < sequence_size; i++){
		sampleFile.read((char*)v, sizeof(double) * dimensionNumber);
		memcpy(pSamples->data+i*dimensionNumber, v,sizeof(double) * dimensionNumber);
	}
	/*
	printf("\n");
	for (int i = 0; i < sequence_size; i++){
		for (int j = 0; j < dimensionNumber; j++){
			printf("%lf ", pSamples->data[(i*dimensionNumber)+j]);
		}
		printf("\n");
	}
	*/
	sampleFile.close();
	//clear
	delete[] v;
	return pSamples;
}
//<sequence_size_M><dimension_N><data_11><data_12>...<data_1N><data_21>...<data_MN>
//一開始假設資料依序分屬於m_stateNumber個狀態的資料,所以均分資料給每個狀態的m_gmms去做初始化
void CHMM::ReadDataBinaryToInitGMMs(const char* fileName)
{
	ifstream sampleFile(fileName, ios_base::binary);
	assert(sampleFile);
	int sequence_size = 0;
	int dimensionNumber = 0;
	sampleFile.read((char*)&sequence_size, sizeof(int));
	//printf("sequence_size=%d\n",sequence_size);
	//gcount()以2bytes為單位ffff, int以4bytes為單位ffff ffff
	//printf("c=%d\n",sampleFile.gcount());
	sampleFile.read((char*)&dimensionNumber, sizeof(int));
	//printf("dimensionNumber=%d\n",dimensionNumber);
	//有sequence_size筆dimensionNumber維度的資料, m_stateNumber個狀態
	//依序切分(sequence_size/m_stateNumber)個資料給每個狀態
	//data_for_each_state[狀態編號][均分到的個數][資料的維度]

	vector<double*> *data_for_each_state = new vector<double*>[m_stateNumber];
	for (int i = 0; i < sequence_size; i++){
		double* v = new double[dimensionNumber];
		sampleFile.read((char*)v, sizeof(double) * dimensionNumber);
		data_for_each_state[i/(sequence_size/m_stateNumber)].push_back(v);
	}
	//用均分的data初始化各個gmms
	for ( int i = 0; i < m_stateNumber; i++)
	{
		m_gmms[i]->Train(data_for_each_state[i],0.00001);		
		//printf("<m_gmms[%d]>\n",i);
		//m_gmms[i]->PrintGMM();
		//printf("</m_gmms[%d]>\n",i);
	}
	//clear
	for ( int i = 0; i < m_stateNumber; i++){
		for(int j =0; j < data_for_each_state[i].size() ; j++){
		delete [] data_for_each_state[i][j];
		}
	} 
}

CHMM::CHMM(int stateNumber, int dimensionNumber, int mixtureNumber)
{
	//初始化 m_C[][], m_pi[], m_A[][]
	isInited = false;
	m_stateNumber = stateNumber;
	m_mixtureNumber = mixtureNumber;
	m_dimensionNumber = dimensionNumber; 
	m_gmms = new GMM*[m_stateNumber];
	m_pi = new double[m_stateNumber];
	m_A = new double*[m_stateNumber];
	for (int i = 0; i < m_stateNumber; i++){
		//初始化GMM
		m_gmms[i] = new GMM(dimensionNumber, mixtureNumber);
		m_A[i] = new double[m_stateNumber + 1]; // 加上最後的狀態
	}
	for (int i = 0; i < m_stateNumber; i++){
		// The initial probabilities
		m_pi[i] = 1.0 / m_stateNumber;
		// The transition probabilities
		for (int j = 0; j <= m_stateNumber; j++)
		{
			m_A[i][j] = 1.0 / (m_stateNumber + 1);
		}
	}
	//
}
CHMM::~CHMM()
{
}
CHMM::CHMM()
{
	isInited = false;
}