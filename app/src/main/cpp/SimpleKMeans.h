#pragma once

typedef unsigned char uchar;

class SimpleKmeans
{
public:
	SimpleKmeans(int dimNum=1,int clusterNum=1);   
	~SimpleKmeans();                                      

	/*ʹ�ø����������ݽ�������  �ٶȵ͵��ǽ����׼ȷ   ����ȡ��ԭ����uchar���� */                      
	void Cluster(float** data,int N,float** result); 
private:

	struct currentCluster{
		currentCluster():changed(true),index(-1){}
		bool changed;  
		int index;      //��ǩ���
	};
	
	int m_dimNum;                     //����ά��
	int m_clusterNum;                 //K��ֵ
	int m_size;                       //���ݵĸ��� 
	currentCluster* m_changeVector;   //�ı��¼����

	bool hasChanged();                //�Ƿ�ı�
    void Init(float** data,float** result);   //��ʼ��
	float CalcDistance(float* color1,float* color2);  //
};
