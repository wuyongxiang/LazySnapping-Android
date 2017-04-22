#pragma  once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SimpleKmeans.h"
#include "maxflow-v3.01/graph.h"

using namespace cv;
using namespace std;

typedef Graph<float,float,float> GraphType_Self;      //����ͼ������

#define ScaleFactor	0x00ffffff    //��������
#define ColorFactor 0.5           //��ɫ����
#define Infinity	0xefffffff    //Ĭ�ϵı�������ǰ����Ȩֵ

const int Self_K=64;              //K��ֵ
const int Self_Dims=3;            //ά��

class LazySnapping_Self
{
private:
	vector<Point> m_forePts;      //����ǰ����
	vector<Point> m_backPts;      //���汳����
	float** m_foreColor;          //�ӱ�����洢������ɫ
	float** m_backColor;          //��ǰ����洢ǰ����ɫ

	uchar** m_avgForColor;        //KMeans�õ���ƽ��ǰ����ɫ
	uchar** m_avgBacColor;        //KMeans�õ���ƽ��������ɫ
	SimpleKmeans* m_kMeans;       //KMeansָ��

	GraphType_Self* m_graph;      //ͼָ��
	//����ԭͼ��������Ϣ
	uchar* m_pSrc;                //ָ��
	int m_Step;                   //����
	int m_Width;                  //���
	int m_Height;                 //�߶�
public:
	LazySnapping_Self();          //���캯��
	~LazySnapping_Self();         //��������
public:
	//�����3���ӿ�  �ֱ��ǳ�ʼ��   ����    ��ȡ�����ģͼ
	void InitLazySnapping(vector<Point>& forPts,vector<Point>& bacPts,Mat& img);
	void Run();
	void GetImageMask(Mat& mask);
private:
	//������ɫ����  ������
	float colorDistance(unsigned char* color1,unsigned char* color2);
	//�������   δ������
	float Distance(unsigned char* color1,unsigned char* color2 );
	//�����Ӧƽ����ɫ������С�ľ���
	float minDistance(unsigned char* color,unsigned char** bfColor);
	//�жϵ��Ƿ��ڶ�Ӧ�ĵ㼯����
	bool isPtInVector(Point pt, vector<Point>& points);
	//��ȡE1��ֵ  ���ص㵽�������ǰ�����Ȩ��
	void GetE1(unsigned char* color,float* energy);
	//��ȡE2��ֵ  ���ص㵽�������ߵ��Ȩ��
	float GetE2(unsigned char* color,unsigned char* color2);
	//����ͼ�ָ�
	void RunMaxFlow();
	//�ͷ���Դ
	void Dispose();
};
