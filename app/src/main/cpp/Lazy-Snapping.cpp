#include "Lazy-Snapping.h"

LazySnapping_Self::LazySnapping_Self()
{
	m_graph=NULL;
	m_pSrc=NULL;
	m_kMeans=NULL;
}

LazySnapping_Self::~LazySnapping_Self()
{
	if (m_graph)      //释放资源
		delete m_graph;
}

void LazySnapping_Self::InitLazySnapping(vector<Point>& forPts,vector<Point>& bacPts,Mat& img)
{
	int i;
	//取得原图像相关信息
	m_pSrc=img.data;
	m_Step=img.step;

	m_Width=img.cols;
	m_Height=img.rows;
	//指定点数目和边数目
	m_graph=new GraphType_Self(m_Width*m_Height,m_Width*m_Height*2);

	//初始化数据部分
	m_forePts.clear();
	for(i=0;i<forPts.size();i++)
	{
		if (!isPtInVector(forPts[i],m_forePts))
			m_forePts.push_back(forPts[i]);
	}
	m_foreColor=new float*[m_forePts.size()];
	for(i=0;i<m_forePts.size();i++)
		m_foreColor[i]=new float[3];

	m_backPts.clear();
	for(i=0;i<bacPts.size();i++)
	{
		if (!isPtInVector(bacPts[i],m_backPts))
			m_backPts.push_back(bacPts[i]);
	}
	m_backColor=new float*[m_backPts.size()];
	for(i=0;i<m_backPts.size();i++)
		m_backColor[i]=new float[3];

	//填充数据 
	int index;
	for(i=0;i<m_forePts.size();i++)
	{
		index=m_forePts[i].y*m_Step+m_forePts[i].x*3;
		m_foreColor[i][0]=float(*(m_pSrc+index));
		m_foreColor[i][1]=float(*(m_pSrc+index+1));
		m_foreColor[i][2]=float(*(m_pSrc+index+2));
	}
	for(i=0;i<m_backPts.size();i++)
	{
		index=m_backPts[i].y*m_Step+m_backPts[i].x*3;
		m_backColor[i][0]=float(*(m_pSrc+index));
		m_backColor[i][1]=float(*(m_pSrc+index+1));
		m_backColor[i][2]=float(*(m_pSrc+index+2));
	}
	//分配结果空间
	m_avgForColor=new uchar*[Self_K];
	m_avgBacColor=new uchar*[Self_K];
	for(i=0;i<Self_K;i++)
	{
		m_avgForColor[i]=new uchar[3];
		m_avgBacColor[i]=new uchar[3];
	}
}

bool LazySnapping_Self::isPtInVector(Point pt, vector<Point>& points)
{
	for(int i=0;i<points.size();i++){
		if(pt.x==points[i].x&&pt.y==points[i].y){
			return true;
		}
	}
	return false;
}

void LazySnapping_Self::Run()
{
	int i,j;
	m_kMeans=new SimpleKmeans(Self_Dims,Self_K);
	float **avgColor;
	avgColor=new float*[Self_K];
	for(i=0;i<Self_K;i++)
		avgColor[i]=new float[3];
	m_kMeans->Cluster(m_foreColor,m_forePts.size(),avgColor);  //运行获取结果
	for(i=0;i<Self_K;i++)
	{
		for(j=0;j<3;j++)
			m_avgForColor[i][j]=saturate_cast<uchar>(avgColor[i][j]);
	}
	m_kMeans->Cluster(m_backColor,m_backPts.size(),avgColor);
	for(i=0;i<Self_K;i++)
	{
		for(j=0;j<3;j++)
			m_avgBacColor[i][j]=saturate_cast<uchar>(avgColor[i][j]);
	}
	RunMaxFlow();   //执行
	//释放空间
	delete m_kMeans;
	for(i=0;i<Self_K;i++)
		delete[] avgColor[i];
	delete[] avgColor;
	Dispose();
}

float LazySnapping_Self::colorDistance(unsigned char* color1,unsigned char* color2)
{
	return sqrt(float((color1[0]-color2[0])*(color1[0]-color2[0])+
					  (color1[1]-color2[1])*(color1[1]-color2[1])+
					  (color1[2]-color2[2])*(color1[2]-color2[2])));
}

float LazySnapping_Self::Distance(unsigned char* color1,unsigned char* color2)
{
	return float((color1[0]-color2[0])*(color1[0]-color2[0])+
				 (color1[1]-color2[1])*(color1[1]-color2[1])+
				 (color1[2]-color2[2])*(color1[2]-color2[2]));
}

float LazySnapping_Self::minDistance(unsigned char* color1,unsigned char** bfColor)
{
	float minDis=Distance(color1,bfColor[0]);
	float distance;
	for(int i=1;i<Self_K;i++)
	{
		distance=Distance(color1,bfColor[i]);
		if (distance<minDis)
			minDis=distance;
	}
	return sqrt(minDis);
}

void LazySnapping_Self::GetE1(unsigned char* color,float* energy)
{
	float df=minDistance(color,m_avgForColor);  //最小距离
	float db=minDistance(color,m_avgBacColor);

	energy[0]=ScaleFactor*(df/(df+db)*ColorFactor+(1-ColorFactor)*0.5);
	energy[1]=ScaleFactor*(db/(df+db)*ColorFactor+(1-ColorFactor)*0.5);
}

float LazySnapping_Self::GetE2(unsigned char* color,unsigned char* color2)
{
	float dis=colorDistance(color,color2);
	float d1 = ScaleFactor*1;
	float d2 = 1+dis;
	return d1/d2;
}

void LazySnapping_Self::RunMaxFlow()
{
	int i,j;
	float e1[2];
	uchar* pSrc;
	int indexPt=0;
	float e2;
	for(i=0;i<m_Height;i++)
	{
		pSrc=m_pSrc+i*m_Step;
		for(j=0;j<m_Width;j++)
		{
			if (isPtInVector(Point(j,i),m_forePts))
			{
				e1[0]=0;
				e1[1]=Infinity;
			}
			else if (isPtInVector(Point(j,i),m_backPts))
			{
				e1[0]=Infinity;
				e1[1]=0;
			}
			else GetE1(pSrc,e1);
			m_graph->add_node();
			m_graph->add_tweights(indexPt,e1[0],e1[1]);
			if (i>0&&j>0)
			{
				e2=GetE2(pSrc,pSrc-3);
				m_graph->add_edge(indexPt,indexPt-1,e2,e2);
				e2=GetE2(pSrc,pSrc-m_Step);
				m_graph->add_edge(indexPt,indexPt-m_Width,e2,e2);
			}
			pSrc+=4;
			indexPt++;
		}
	}
	m_graph->maxflow();  //分割
}
void LazySnapping_Self::GetImageMask(Mat& mask)
{
	if (!mask.data)
	{
		mask=Mat::zeros(m_Height,m_Width,CV_8UC1);
	}
	int indexpt=0;
	int i,j;
	int num = m_Height*m_Width;
	for(i=0;i<m_Height;i++)
	{
		unsigned char* p=mask.ptr<uchar>(i);
		for(j=0;j<m_Width;j++)
		{
			if (m_graph->what_segment(indexpt) == GraphType_Self::SOURCE){
                *p = 0;
			}else{
                *p = 255;
			}
			p++;
			indexpt++;
		}
	}

}

void LazySnapping_Self::Dispose()
{
	m_forePts.clear();
	m_backPts.clear();
	for(int i=0;i<Self_K;i++)
	{
		delete []m_avgForColor[i];
		delete []m_avgBacColor[i];
	}
	delete[] m_avgBacColor;
	delete[] m_avgForColor;
}
