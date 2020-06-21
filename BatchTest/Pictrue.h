#pragma once

#include "CApplication.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
#include "CRange.h"
#include "CFont0.h"

#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

#include<opencv.hpp>
#include<vector>

using namespace std;
using namespace cv;

class Pictrue
{
public:
	Pictrue();
	~Pictrue();

	// 保存一条图像结果
	long valueCount;            // 数量
	double meanArea;            // 平均面积结果
	double meanLength;          // 平均周长结果
	double ratioArea;           // 计算的面积率
	double meanMinDia;          // 平均最小粒径
	double meanMaxDia;          // 平均最大粒径
	double maxBigDia;           // 所有最大大径
	double minBigDia;           // 所有最小大径
	double maxSmallDia;         // 所有最大小径
	double minSmallDia;         // 所有最小小径
	double maxLength;           // 所有最大周长
	double minLength;           // 所有最小周长
	double maxArea;             // 所有最大面积
	double minArea;             // 所有最小面积
	double sumBigDia;           // 最大直径之和
	double sumSmallDia;         // 最小直径之和
	double sumLength;           // 周长和
	double sumArea;             // 面积和
	double bigDiaSD;            // 最大直径标准差
	double smallDiaSD;          // 最小直径标准差
	double lengthSD;            // 周长的标准差
	double areaSD;              // 面积的标准差
	double areaStrip;           // 每一条的面积
	vector<double> vecArea;       // 面积
	vector<double> vecLength;     // 周长
	vector<double> vecMaxDia;     // 每个最大粒径
	vector<double> vecMinDia;     // 每个最小粒径
	CString PicSavePath;           //保存结果图路径
	CString ExcelSavePath;         //保存excel结果路径

	vector<CString> vecPicPaths;    //所选图片路径容器
	double resolution;     // 分辨率
	int diaThreshold_low;     // 低粒径阈值
	int diaThreshold_high;     // 高粒径阈值
	int grayThreshold;     // 灰度阈值（减数）
	int perimeterThreshold;   //周长阈值
	int radio;              //最小外接矩形长宽比
	CString savePath;     //保存路径

	// 结果写入相关变量
	CApplication app;
	CWorkbook book;
	CWorkbooks books;
	CWorksheet sheet;
	CWorksheets sheets;
	CRange range;
	CFont0 font;
	//COleVariant vResult;
	//LPDISPATCH lpDisp;
	VARIANT val;

	int m_perimeterThreshold;   //周长阈值
	void SetParameter(double m_resolution, int m_diaThreshold_low, int m_diaThreshold_high, int m_grayThreshold, int m_perimeterThreshold,int m_radio, CString m_savePath, vector<CString> vecPicPaths);   //设置参数（从主类中传进参数）
	void Initialize();   //参数初始化
	void ComputeAccuracy(Mat srcImg);     // 保存结果图并精确计算出各项参数
	int OtsuAlgThreshold(Mat image);     // 计算Otsu阈值
	void WriteResultToExcelAccuracy();        // 将结果保存到excel文件
	void SetFilePath(CString picpath, CString savepath);    //得到两个结果路径
	//void Progressing();   //总处理
};

