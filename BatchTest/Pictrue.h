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

	// ����һ��ͼ����
	long valueCount;            // ����
	double meanArea;            // ƽ��������
	double meanLength;          // ƽ���ܳ����
	double ratioArea;           // ����������
	double meanMinDia;          // ƽ����С����
	double meanMaxDia;          // ƽ���������
	double maxBigDia;           // ��������
	double minBigDia;           // ������С��
	double maxSmallDia;         // �������С��
	double minSmallDia;         // ������СС��
	double maxLength;           // ��������ܳ�
	double minLength;           // ������С�ܳ�
	double maxArea;             // ����������
	double minArea;             // ������С���
	double sumBigDia;           // ���ֱ��֮��
	double sumSmallDia;         // ��Сֱ��֮��
	double sumLength;           // �ܳ���
	double sumArea;             // �����
	double bigDiaSD;            // ���ֱ����׼��
	double smallDiaSD;          // ��Сֱ����׼��
	double lengthSD;            // �ܳ��ı�׼��
	double areaSD;              // ����ı�׼��
	double areaStrip;           // ÿһ�������
	vector<double> vecArea;       // ���
	vector<double> vecLength;     // �ܳ�
	vector<double> vecMaxDia;     // ÿ���������
	vector<double> vecMinDia;     // ÿ����С����
	CString PicSavePath;           //������ͼ·��
	CString ExcelSavePath;         //����excel���·��

	vector<CString> vecPicPaths;    //��ѡͼƬ·������
	double resolution;     // �ֱ���
	int diaThreshold_low;     // ��������ֵ
	int diaThreshold_high;     // ��������ֵ
	int grayThreshold;     // �Ҷ���ֵ��������
	int perimeterThreshold;   //�ܳ���ֵ
	int radio;              //��С��Ӿ��γ����
	CString savePath;     //����·��

	// ���д����ر���
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

	int m_perimeterThreshold;   //�ܳ���ֵ
	void SetParameter(double m_resolution, int m_diaThreshold_low, int m_diaThreshold_high, int m_grayThreshold, int m_perimeterThreshold,int m_radio, CString m_savePath, vector<CString> vecPicPaths);   //���ò������������д���������
	void Initialize();   //������ʼ��
	void ComputeAccuracy(Mat srcImg);     // ������ͼ����ȷ������������
	int OtsuAlgThreshold(Mat image);     // ����Otsu��ֵ
	void WriteResultToExcelAccuracy();        // ��������浽excel�ļ�
	void SetFilePath(CString picpath, CString savepath);    //�õ��������·��
	//void Progressing();   //�ܴ���
};

