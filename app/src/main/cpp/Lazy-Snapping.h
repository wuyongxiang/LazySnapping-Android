#pragma  once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SimpleKmeans.h"
#include "maxflow-v3.01/graph.h"

using namespace cv;
using namespace std;

typedef Graph<float,float,float> GraphType_Self;      //定义图的类型

#define ScaleFactor	0x00ffffff    //比例常数
#define ColorFactor 0.5           //颜色比例
#define Infinity	0xefffffff    //默认的背景或者前景的权值

const int Self_K=64;              //K的值
const int Self_Dims=3;            //维度

class LazySnapping_Self
{
private:
	vector<Point> m_forePts;      //保存前景点
	vector<Point> m_backPts;      //保存背景点
	float** m_foreColor;          //从背景点存储背景颜色
	float** m_backColor;          //从前景点存储前景颜色

	uchar** m_avgForColor;        //KMeans得到的平均前景颜色
	uchar** m_avgBacColor;        //KMeans得到的平均背景颜色
	SimpleKmeans* m_kMeans;       //KMeans指针

	GraphType_Self* m_graph;      //图指针
	//保存原图像的相关信息
	uchar* m_pSrc;                //指针
	int m_Step;                   //步长
	int m_Width;                  //宽度
	int m_Height;                 //高度
public:
	LazySnapping_Self();          //构造函数
	~LazySnapping_Self();         //析构函数
public:
	//对外的3个接口  分别是初始化   运行    获取结果掩模图
	void InitLazySnapping(vector<Point>& forPts,vector<Point>& bacPts,Mat& img);
	void Run();
	void GetImageMask(Mat& mask);
private:
	//计算颜色距离  开根号
	float colorDistance(unsigned char* color1,unsigned char* color2);
	//计算距离   未开根号
	float Distance(unsigned char* color1,unsigned char* color2 );
	//计算对应平均颜色里面最小的距离
	float minDistance(unsigned char* color,unsigned char** bfColor);
	//判断点是否在对应的点集合中
	bool isPtInVector(Point pt, vector<Point>& points);
	//获取E1的值  像素点到背景点和前景点的权重
	void GetE1(unsigned char* color,float* energy);
	//获取E2的值  像素点到上面和左边点的权重
	float GetE2(unsigned char* color,unsigned char* color2);
	//运行图分割
	void RunMaxFlow();
	//释放资源
	void Dispose();
};
