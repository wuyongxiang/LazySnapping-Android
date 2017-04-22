#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include "SimpleKMeans.h"

using namespace std;

SimpleKmeans::SimpleKmeans(int dimNum, int clusterNum)
{
	m_dimNum = dimNum;
	m_clusterNum = clusterNum;
}

SimpleKmeans::~SimpleKmeans()
{  
	delete []m_changeVector;
}

void SimpleKmeans::Cluster(float** data,int N,float** result)
{
	m_size=N;

	Init(data,result);

    int i,j,index;
	float minVal,val;
	int* counts = new int[m_clusterNum];           
	float** next_means = new float*[m_clusterNum];   
	for (i=0;i<m_clusterNum;i++)
	{
		next_means[i]=new float[m_dimNum];
	}

	while (hasChanged())
	{   
		memset(counts, 0, sizeof(int) * m_clusterNum);
		for (i = 0; i < m_clusterNum; i++)
		{
			memset(next_means[i], 0, m_dimNum*sizeof(float));
		}

		for (i = 0; i < m_size; i++)
		{
			 minVal=CalcDistance(result[0],data[i]);  
			 index=0;
			 for(j=1;j<m_clusterNum;j++)
			 {
				 val=CalcDistance(result[j],data[i]); //最小距离
				 if (val<minVal)
				 {
					 minVal=val;
					 index=j;
				 }
			 }
			 for(j=0;j<3;j++)
				 next_means[index][j]+=data[i][j];
			 counts[index]++;
			 m_changeVector[i].changed=(m_changeVector[i].index!=index)?true:false;
			 m_changeVector[i].index=index;  //标记
		}
		for(i=0;i<m_clusterNum;i++)
		{
			if (counts[i]>0)
			{
				for(j=0;j<3;j++)
					result[i][j]=next_means[i][j]/counts[i];  //重新赋值 
			}
		}
	}


	delete[] counts;
	for (int i = 0; i < m_clusterNum; i++)
		delete[] next_means[i];
	delete[] next_means;

	for(int i=0;i<m_size;i++)
		delete[] data[i];
	delete []data;
}

void SimpleKmeans::Init(float** data,float** result)
{   
	m_changeVector=new currentCluster[m_size];

    int inteval = m_size/m_clusterNum;       
	int select,i,j;
	//初始化
	for (i=0;i<m_clusterNum;i++)
	{
	     select=i*inteval;    
	     for(j=0;j<m_dimNum;j++)   
			 result[i][j]=data[select][j];
	}
}

float SimpleKmeans::CalcDistance(float* color1,float* color2)
{
	return (color1[0]-color2[0])*(color1[0]-color2[0])+
		   (color1[1]-color2[1])*(color1[1]-color2[1])+         
		   (color1[2]-color2[2])*(color1[2]-color2[2]);
}

bool SimpleKmeans::hasChanged()
{
	for(int i=0;i<m_size;i++)
		if (m_changeVector[i].changed)
			return true;
	return false;
}