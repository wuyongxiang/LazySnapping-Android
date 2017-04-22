#pragma once

typedef unsigned char uchar;

class SimpleKmeans
{
public:
	SimpleKmeans(int dimNum=1,int clusterNum=1);   
	~SimpleKmeans();                                      

	/*使用浮点数的数据进行运算  速度低但是结果更准确   这里取消原来的uchar类型 */                      
	void Cluster(float** data,int N,float** result); 
private:

	struct currentCluster{
		currentCluster():changed(true),index(-1){}
		bool changed;  
		int index;      //标签标记
	};
	
	int m_dimNum;                     //数据维数
	int m_clusterNum;                 //K的值
	int m_size;                       //数据的个数 
	currentCluster* m_changeVector;   //改变记录向量

	bool hasChanged();                //是否改变
    void Init(float** data,float** result);   //初始化
	float CalcDistance(float* color1,float* color2);  //
};
