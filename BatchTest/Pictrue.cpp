#include "stdafx.h"
#include "Pictrue.h"


Pictrue::Pictrue()
{
}


Pictrue::~Pictrue()
{
}


//设置参数（从主类中传进参数）
void Pictrue::SetParameter(double m_resolution, int m_diaThreshold_low, int m_diaThreshold_high, int m_grayThreshold, int m_perimeterThreshold, int m_radio, CString m_savePath, vector<CString> PicPaths)
{
	resolution = m_resolution;
	diaThreshold_low = m_diaThreshold_low;
	diaThreshold_high = m_diaThreshold_high;
	grayThreshold = m_grayThreshold;
	perimeterThreshold = m_perimeterThreshold;
	radio = m_radio;
	savePath = m_savePath;
	vecPicPaths = PicPaths;
}


//参数初始化
void Pictrue::Initialize()
{
	valueCount = 0;            // 数量
	meanArea = 0;            // 平均面积结果
	meanLength = 0;          // 平均周长结果
	ratioArea = 0;           // 计算的面积率
	meanMinDia = 0;          // 平均最小粒径
	meanMaxDia = 0;          // 平均最大粒径
	maxBigDia = 0;           // 所有最大大径
	minBigDia = 0;           // 所有最小大径
	maxSmallDia = 0;         // 所有最大小径
	minSmallDia = 0;         // 所有最小小径
	maxLength = 0;           // 所有最大周长
	minLength = 0;           // 所有最小周长
	maxArea = 0;             // 所有最大面积
	minArea = 0;             // 所有最小面积
	sumBigDia = 0;           // 最大直径之和
	sumSmallDia = 0;         // 最小直径之和
	sumLength = 0;           // 周长和
	sumArea = 0;             // 面积和
	bigDiaSD = 0;            // 最大直径标准差
	smallDiaSD = 0;          // 最小直径标准差
	lengthSD = 0;            // 周长的标准差
	areaSD = 0;              // 面积的标准差
	areaStrip = 0;           // 每一条的面积
	vecArea.clear();         // 面积
	vecLength.clear();       // 周长
	vecMaxDia.clear();       // 每个最大粒径
	vecMinDia.clear();       // 每个最小粒径
	PicSavePath = "";           //保存结果图路径
	ExcelSavePath = "";         //保存excel结果路径
}


// 精确计算函数
void Pictrue::ComputeAccuracy(Mat srcImg)
{
	Mat dstImg;
	int thresholdValue = OtsuAlgThreshold(srcImg);
	thresholdValue = thresholdValue - grayThreshold;
	threshold(srcImg, dstImg, thresholdValue, 255, CV_THRESH_BINARY_INV);

	// 结构元
	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(2, 2));
	// 开运算
	morphologyEx(dstImg, dstImg, MORPH_OPEN, element1);
	// 闭运算
	morphologyEx(dstImg, dstImg, MORPH_CLOSE, element2);

	// 轮廓查找
	vector < vector < cv::Point >> contours;
	vector<Vec4i> hierarchy;
	findContours(dstImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat dstImg2 = Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC3);

	Scalar color_red(0, 0, 255);
	Scalar color_green(0, 255, 0);
	Scalar color_blue(255, 0, 0);
	Scalar color_white(255, 255, 255);
	double area = 0;   //每个颗粒的面积（临时参数）
	double length = 0;   //每个颗粒的周长（临时参数）
	double areaSum = 0.0;   //总面积（临时参数）
	double lengthSum = 0.0;   //总周长（临时参数）
	double maxRadius = 0.0;   // 最大粒径
	double minRadius = 0.0;   // 最小粒径
	double radiusMaxSum = 0.0;   // 最大粒径之和
	double radiusMinSum = 0.0;   // 最小粒径之和
	// 最小外接矩形
	double l1 = 0.0;
	double l2 = 0.0;
	double bigL = 0.0;   //长
	double smallL = 0.0;   //宽


	// 遍历所有顶层轮廓
	for (auto index = 0; index < contours.size(); index++)
	{
		area = contourArea(contours[index]);
		length = arcLength(contours[index], true);

		// 最小外接矩形计算
		RotatedRect rect = minAreaRect(contours[index]);
		Point2f P[4];
		rect.points(P);
		Size s = rect.size;
		l1 = s.width;
		l2 = s.height;
		if (l1 > l2)
		{
			bigL = l1 * resolution;
			smallL = l2 * resolution;
		}
		else
		{
			bigL = l2 * resolution;
			smallL = l1 * resolution;
		}
		minRadius = bigL;  // 得到最大粒径

		//粒径在阈值范围才进行处理
		if (minRadius >= diaThreshold_low && minRadius <= diaThreshold_high)
		{
			//int ratio_1 = bigL / smallL;
			//int ratio_2 = (int)length * (int)length / (int)area;  //利用周长平方与面积之比来滤除条状纹理
			if (length * resolution > perimeterThreshold)
			{
				cv::drawContours(dstImg2, contours, index, color_white, CV_FILLED, 8, hierarchy);
				areaStrip -= area;
			}
			else if (bigL / smallL > radio)
			{
				cv::drawContours(dstImg2, contours, index, color_green, CV_FILLED, 8, hierarchy);
				areaStrip -= area;
			}
			else
			{
				double maxLength = 0.0;
				double curLenth = 0.0;
				// 每个颗粒内循环精确计算最大粒径
				for (auto i = 0; i < contours[index].size(); i++)
				{
					for (auto j = 0; j < contours[index].size(); j++)
					{
						curLenth = sqrt((contours[index][i].x - contours[index][j].x)*(contours[index][i].x - contours[index][j].x) +
							(contours[index][i].y - contours[index][j].y)*(contours[index][i].y - contours[index][j].y));
						if (curLenth > maxLength)
							maxLength = curLenth;
					}
				}
				maxRadius = maxLength * resolution;// 得到最大粒径

				valueCount++;
				areaSum += area;
				lengthSum += length;
				radiusMaxSum += maxRadius;
				radiusMinSum += minRadius;

				area = area * resolution*resolution;
				length = length * resolution;

				vecArea.push_back(area);           // 面积
				vecLength.push_back(length);       // 周长
				vecMaxDia.push_back(maxRadius);    // 每个最大粒径
				vecMinDia.push_back(minRadius);    // 每个最小粒径
				cv::drawContours(dstImg2, contours, index, color_red, CV_FILLED, 8, hierarchy);
			}
		}
	}


	//画刻度线，1mm标小刻度，5mm标大刻度
	for (int m = 1; double(1000 * m / resolution) < dstImg2.cols; m++)
	{
		if (m % 5 == 0)
			line(dstImg2, Point(double(1000 * m / resolution), 0), Point(double(1000 * m / resolution), 80), color_white, 11, 8);
		else
			line(dstImg2, Point(double(1000 * m / resolution), 0), Point(double(1000 * m / resolution), 30), color_white, 7, 8);
	}
	for (int n = 1; 1000 * n / resolution < dstImg2.rows; n++)
	{
		if (n % 5 == 0)
			line(dstImg2, Point(0, double(1000 * n / resolution)), Point(80, double(1000 * n / resolution)), color_white, 11, 8);
		else
			line(dstImg2, Point(0, double(1000 * n / resolution)), Point(30, double(1000 * n / resolution)), color_white, 7, 8);
	}


	USES_CONVERSION;
	cv::String PicSavePath_cv = W2A(PicSavePath);
	cv::imwrite(PicSavePath_cv, dstImg2);

	// ******************【平均】 【面积率】 【区域整体面积】
	if (valueCount != 0)
	{
		meanMaxDia = radiusMaxSum / valueCount;                                 // 平均最大粒径
		meanMinDia = radiusMinSum / valueCount;                                 // 平均最小粒径
		meanArea = areaSum / valueCount * resolution * resolution;              // 平均面积
		meanLength = lengthSum / valueCount * resolution;                       // 平均周长
		areaStrip += srcImg.rows*srcImg.cols;                                   // 一小条的像素面积
		ratioArea = areaSum / areaStrip * 100;                                  // 面积率
		areaStrip = areaStrip * resolution*resolution;             // 一小条的面积

		// ******************【总计】
		sumBigDia = radiusMaxSum;
		sumSmallDia = radiusMinSum;
		sumLength = lengthSum * resolution;
		sumArea = areaSum * resolution*resolution;

		// *****************【最大】 【最小】 【标准差】
		for (auto i = 0; i < vecArea.size(); i++)
		{
			// 循环第一次初始化
			if (i == 0)
			{
				maxBigDia = vecMaxDia[i];
				minBigDia = vecMaxDia[i];

				maxSmallDia = vecMinDia[i];
				minSmallDia = vecMinDia[i];

				maxLength = vecLength[i];
				minLength = vecLength[i];

				maxArea = vecArea[i];
				minArea = vecArea[i];
			}
			else
			{
				if (maxBigDia < vecMaxDia[i])
					maxBigDia = vecMaxDia[i];
				if (minBigDia > vecMaxDia[i])
					minBigDia = vecMaxDia[i];

				if (maxSmallDia < vecMinDia[i])
					maxSmallDia = vecMinDia[i];
				if (minSmallDia > vecMinDia[i])
					minSmallDia = vecMinDia[i];

				if (maxLength < vecLength[i])
					maxLength = vecLength[i];
				if (minLength > vecLength[i])
					minLength = vecLength[i];

				if (maxArea < vecArea[i])
					maxArea = vecArea[i];
				if (minArea > vecArea[i])
					minArea = vecArea[i];

			}
			// 标准差的计算
			bigDiaSD += (vecMaxDia[i] - meanMaxDia)*(vecMaxDia[i] - meanMaxDia) / valueCount;
			smallDiaSD += (vecMinDia[i] - meanMinDia)*(vecMinDia[i] - meanMinDia) / valueCount;
			lengthSD += (vecLength[i] - meanLength)*(vecLength[i] - meanLength) / valueCount;
			areaSD += (vecArea[i] - meanArea)*(vecArea[i] - meanArea) / valueCount;
		}

		bigDiaSD = sqrt(bigDiaSD);
		smallDiaSD = sqrt(smallDiaSD);
		lengthSD = sqrt(lengthSD);
		areaSD = sqrt(areaSD);
	}
}


// 计算Otsu阈值
int Pictrue::OtsuAlgThreshold(Mat image)
{
	int T = 0; //Otsu算法阈值  
	double varValue = 0; //类间方差中间值保存  
	double w0 = 0; //前景像素点数所占比例  
	double w1 = 0; //背景像素点数所占比例  
	double u0 = 0; //前景平均灰度  
	double u1 = 0; //背景平均灰度  
	double Histogram[256] = { 0 }; //灰度直方图，下标是灰度值，保存内容是灰度值对应的像素点总数  
	unsigned char *data = image.data;
	double totalNum = image.rows*image.cols; //像素总数  

											 //计算灰度直方图分布，Histogram数组下标是灰度值，保存内容是灰度值对应像素点数  
	for (int i = 0; i < image.rows; i++)   //为表述清晰，并没有把rows和cols单独提出来  
	{
		for (int j = 0; j < image.cols; j++)
		{
			Histogram[data[i*image.step + j]]++;
		}
	}
	for (int i = 0; i < 255; i++)
	{
		//每次遍历之前初始化各变量  
		w1 = 0;
		u1 = 0;
		w0 = 0;
		u0 = 0;
		//***********背景各分量值计算**************************  
		for (int j = 0; j <= i; j++) //背景部分各值计算  
		{
			w1 += Histogram[j];  //背景部分像素点总数  
			u1 += j * Histogram[j]; //背景部分像素总灰度和  
		}
		//if (w1 == 0) //背景部分像素点数为0时退出  
		//{
		//	break;
		//}
		u1 = u1 / w1; //背景像素平均灰度  
		w1 = w1 / totalNum; // 背景部分像素点数所占比例  


		//***********前景各分量值计算**************************  
		for (int k = i + 1; k < 255; k++)
		{
			w0 += Histogram[k];  //前景部分像素点总数  
			u0 += k * Histogram[k]; //前景部分像素总灰度和  
		}
		if (w0 == 0) //前景部分像素点数为0时退出  
		{
			break;
		}
		u0 = u0 / w0; //前景像素平均灰度  
		w0 = w0 / totalNum; // 前景部分像素点数所占比例  
							//***********前景各分量值计算**************************  

							//***********类间方差计算******************************  
		double varValueI = w0 * w1*(u1 - u0)*(u1 - u0); //当前类间方差计算  
		if (varValue < varValueI)
		{
			varValue = varValueI;
			T = i;
			// cout << T << endl;
		}
	}
	return T;
}


// 将结果保存到excel文件
void Pictrue::WriteResultToExcelAccuracy()

{
	// 初始化组件
	CoUninitialize();
	if (CoInitialize(NULL) == S_FALSE)
	{
		AfxMessageBox(_T("初始化COM支持库失败！"));
	}

	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (!app.CreateDispatch(_T("Excel.Application")))
	{
		AfxMessageBox(_T("无法创建Excel应用！"));
	}

	books = app.get_Workbooks();
	book = books.Add(covOptional);
	sheets = book.get_Worksheets();
	sheet = sheets.get_Item(COleVariant((short)1));

	//居中对齐方式
	val.vt = VT_I2;
	val.iVal = -4108;


	// *****************************【表头】********************************
	//获得坐标为（A，1）和（D，1）的两个单元格 
	range = sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("E1")));
	range.put_RowHeight(_variant_t((long)50));  //设置列宽
	range.Merge(_variant_t((long)0));//合并单元格

	//居中对齐
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);

	//设置单元格内容
	range.put_Value2(COleVariant(_T("结果统计报告")));

	//设置字体粗体
	font = range.get_Font();
	font.put_Bold(COleVariant((short)TRUE));
	font.put_Name(_variant_t("宋体"));
	font.put_Size(_variant_t(30));


	// *************************【保存时间】********************************
	range = sheet.get_Range(COleVariant(_T("A2")), COleVariant(_T("A2")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("保存时间：")));

	CTime curTime = CTime::GetCurrentTime();
	CString curTimeCStr;

	curTimeCStr = curTime.Format(_T("%Y.%m.%d"));
	range = sheet.get_Range(COleVariant(_T("B2")), COleVariant(_T("B2")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(curTimeCStr));

	curTimeCStr = curTime.Format(_T("%H:%M"));
	range = sheet.get_Range(COleVariant(_T("C2")), COleVariant(_T("C2")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(curTimeCStr));


	// *************************【统计结果】********************************
	range = sheet.get_Range(COleVariant(_T("A3")), COleVariant(_T("A3")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("【统计】")));

	range = sheet.get_Range(COleVariant(_T("B4")), COleVariant(_T("B4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最大直径/um")));

	range = sheet.get_Range(COleVariant(_T("C4")), COleVariant(_T("C4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最小直径/um")));

	range = sheet.get_Range(COleVariant(_T("D4")), COleVariant(_T("D4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("周长/um")));

	range = sheet.get_Range(COleVariant(_T("E4")), COleVariant(_T("E4")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("面积/um2")));

	range = sheet.get_Range(COleVariant(_T("A5")), COleVariant(_T("A5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("平均")));

	range = sheet.get_Range(COleVariant(_T("A6")), COleVariant(_T("A6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("标准差")));

	range = sheet.get_Range(COleVariant(_T("A7")), COleVariant(_T("A7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最大")));

	range = sheet.get_Range(COleVariant(_T("A8")), COleVariant(_T("A8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最小")));

	range = sheet.get_Range(COleVariant(_T("A9")), COleVariant(_T("A9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("总计")));



	// ******************【平均】
	// 最大直径
	CString strResult;
	strResult.Format(_T("%.0f"), meanMaxDia);
	range = sheet.get_Range(COleVariant(_T("B5")), COleVariant(_T("B5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 最小直径
	strResult.Format(_T("%.0f"), meanMinDia);
	range = sheet.get_Range(COleVariant(_T("C5")), COleVariant(_T("C5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 周长
	strResult.Format(_T("%.0f"), meanLength);
	range = sheet.get_Range(COleVariant(_T("D5")), COleVariant(_T("D5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 面积
	strResult.Format(_T("%.0f"), meanArea);
	range = sheet.get_Range(COleVariant(_T("E5")), COleVariant(_T("E5")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ******************【标准差】
	// 最大直径
	strResult.Format(_T("%.0f"), bigDiaSD);
	range = sheet.get_Range(COleVariant(_T("B6")), COleVariant(_T("B6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 最小直径
	strResult.Format(_T("%.0f"), smallDiaSD);
	range = sheet.get_Range(COleVariant(_T("C6")), COleVariant(_T("C6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 周长
	strResult.Format(_T("%.0f"), lengthSD);
	range = sheet.get_Range(COleVariant(_T("D6")), COleVariant(_T("D6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 面积
	strResult.Format(_T("%.0f"), areaSD);
	range = sheet.get_Range(COleVariant(_T("E6")), COleVariant(_T("E6")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));


	// ******************【最大】
	// 最大直径
	strResult.Format(_T("%.0f"), maxBigDia);
	range = sheet.get_Range(COleVariant(_T("B7")), COleVariant(_T("B7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 最小直径
	strResult.Format(_T("%.0f"), maxSmallDia);
	range = sheet.get_Range(COleVariant(_T("C7")), COleVariant(_T("C7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 周长
	strResult.Format(_T("%.0f"), maxLength);
	range = sheet.get_Range(COleVariant(_T("D7")), COleVariant(_T("D7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 面积
	strResult.Format(_T("%.0f"), maxArea);
	range = sheet.get_Range(COleVariant(_T("E7")), COleVariant(_T("E7")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ******************【最小】
	// 最大直径
	strResult.Format(_T("%.0f"), minBigDia);
	range = sheet.get_Range(COleVariant(_T("B8")), COleVariant(_T("B8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 最小直径
	strResult.Format(_T("%.0f"), minSmallDia);
	range = sheet.get_Range(COleVariant(_T("C8")), COleVariant(_T("C8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 周长
	strResult.Format(_T("%.0f"), minLength);
	range = sheet.get_Range(COleVariant(_T("D8")), COleVariant(_T("D8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 面积
	strResult.Format(_T("%.0f"), minArea);
	range = sheet.get_Range(COleVariant(_T("E8")), COleVariant(_T("E8")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// ******************【总计】
	// 最大直径
	strResult.Format(_T("%.0f"), sumBigDia);
	range = sheet.get_Range(COleVariant(_T("B9")), COleVariant(_T("B9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 最小直径
	strResult.Format(_T("%.0f"), sumSmallDia);
	range = sheet.get_Range(COleVariant(_T("C9")), COleVariant(_T("C9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 周长
	strResult.Format(_T("%.0f"), sumLength);
	range = sheet.get_Range(COleVariant(_T("D9")), COleVariant(_T("D9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));

	// 面积
	strResult.Format(_T("%.0f"), sumArea);
	range = sheet.get_Range(COleVariant(_T("E9")), COleVariant(_T("E9")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strResult));


	// *************************【主信息】********************************
	range = sheet.get_Range(COleVariant(_T("A13")), COleVariant(_T("A13")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("【主信息】")));

	range = sheet.get_Range(COleVariant(_T("A14")), COleVariant(_T("A14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("No.")));

	range = sheet.get_Range(COleVariant(_T("B14")), COleVariant(_T("B14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最大直径/um")));

	range = sheet.get_Range(COleVariant(_T("C14")), COleVariant(_T("C14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("最小直径/um")));

	range = sheet.get_Range(COleVariant(_T("D14")), COleVariant(_T("D14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("周长/um")));

	range = sheet.get_Range(COleVariant(_T("E14")), COleVariant(_T("E14")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("面积/um2")));


	long irow;
	CRange start_range, write_range;
	CString strValue;
	long sizeVec = vecArea.size();
	for (irow = 1; irow <= sizeVec; irow++)
	{
		// No.
		strValue.Format(_T("%ld"), irow);
		COleVariant new_value0(strValue);
		start_range = sheet.get_Range(COleVariant(_T("A14")), covOptional);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)0));
		write_range.put_Value2(new_value0);
		write_range.put_HorizontalAlignment(val);

		// 最大直径/um
		strValue.Format(_T("%.0f"), vecMaxDia[irow - 1]);
		COleVariant new_value1(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)1));
		write_range.put_Value2(new_value1);
		write_range.put_HorizontalAlignment(val);

		// 最小直径/um
		strValue.Format(_T("%.0f"), vecMinDia[irow - 1]);
		COleVariant new_value2(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)2));
		write_range.put_Value2(new_value2);
		write_range.put_HorizontalAlignment(val);

		// 周长/um
		strValue.Format(_T("%.0f"), vecLength[irow - 1]);
		COleVariant new_value3(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)3));
		write_range.put_Value2(new_value3);
		write_range.put_HorizontalAlignment(val);

		// 面积/um2
		strValue.Format(_T("%.0f"), vecArea[irow - 1]);
		COleVariant new_value4(strValue);
		write_range = start_range.get_Offset(COleVariant((long)irow), COleVariant((long)4));
		write_range.put_Value2(new_value4);
		write_range.put_HorizontalAlignment(val);
	}


	// 值
	start_range = sheet.get_Range(COleVariant(_T("A14")), covOptional);
	write_range = start_range.get_Offset(COleVariant((long)irow + 1), COleVariant((long)1));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("值")));

	// 单位
	write_range = start_range.get_Offset(COleVariant((long)irow + 1), COleVariant((long)2));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("单位")));


	// 总面积
	write_range = start_range.get_Offset(COleVariant((long)irow + 2), COleVariant((long)0));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("总面积")));

	strValue.Format(_T("%.0f"), meanArea*valueCount);
	COleVariant new_value_areaSum(strValue);
	write_range = start_range.get_Offset(COleVariant((long)irow + 2), COleVariant((long)1));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(new_value_areaSum));

	write_range = start_range.get_Offset(COleVariant((long)irow + 2), COleVariant((long)2));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("um2")));


	// 区域整体面积
	write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)0));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("区域整体面积")));

	strValue.Format(_T("%.0f"), areaStrip);
	COleVariant new_value_areaStrip(strValue);
	write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)1));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(new_value_areaStrip));

	write_range = start_range.get_Offset(COleVariant((long)irow + 3), COleVariant((long)2));
	write_range.put_ColumnWidth(_variant_t((long)12));
	write_range.put_HorizontalAlignment(val);
	write_range.put_Value2(COleVariant(_T("um2")));


	// 计数
	range = sheet.get_Range(COleVariant(_T("A10")), COleVariant(_T("A10")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("计数/个")));

	strValue.Format(_T("%ld"), valueCount);
	range = sheet.get_Range(COleVariant(_T("B10")), COleVariant(_T("B10")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strValue));



	//面积率
	range = sheet.get_Range(COleVariant(_T("A11")), COleVariant(_T("A11")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(_T("面积率/%")));

	strValue.Format(_T("%.2f"), ratioArea);
	range = sheet.get_Range(COleVariant(_T("B11")), COleVariant(_T("B11")));
	range.put_ColumnWidth(_variant_t((long)12));
	range.put_HorizontalAlignment(val);
	range.put_VerticalAlignment(val);
	range.put_Value2(COleVariant(strValue));


	//表格显示
	//app.put_Visible(TRUE);
	book.put_Saved(false);

	//不显示提示对话框
	app.put_DisplayAlerts(false);


	//保存excel
	//book.SaveAs(COleVariant(ExcelSavePath),
	//	_variant_t(39),        //07版
	//	_variant_t(vtMissing),
	//	_variant_t(vtMissing),
	//	_variant_t(vtMissing),
	//	_variant_t(vtMissing),
	//	0,
	//	_variant_t(vtMissing),
	//	_variant_t(vtMissing),
	//	_variant_t(vtMissing),
	//	_variant_t(vtMissing),
	//	_variant_t(vtMissing));
	book.SaveCopyAs(COleVariant(ExcelSavePath));
	book.put_Saved(TRUE);

	//结尾，释放

	//start_range.ReleaseDispatch();
	//write_range.ReleaseDispatch();

	//book.ReleaseDispatch();
	//books.ReleaseDispatch();
	//sheet.ReleaseDispatch();
	//sheets.ReleaseDispatch();
	//range.ReleaseDispatch();
	//font.ReleaseDispatch();
	//book.Close(covOptional, covOptional, covOptional);	   // 关闭Workbook对象
	//books.Close();										   // 关闭Workbooks对象

	//app.Quit();
	//app.ReleaseDispatch();

	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();

	start_range.ReleaseDispatch();
	write_range.ReleaseDispatch();

	book.ReleaseDispatch();								   // 释放Workbook对象
	books.ReleaseDispatch();							   // 释放Workbooks对象
	book.Close(covOptional, covOptional, covOptional);	   // 关闭Workbook对象
	books.Close();										   // 关闭Workbooks对象

	app.Quit();
	app.ReleaseDispatch();
}


//得到两个结果路径
void Pictrue::SetFilePath(CString picpath, CString savepath)
{
	CString FileName, FileSuffix;
	FileName = picpath.Right(picpath.GetLength() - picpath.ReverseFind('\\') - 1);//从路径中截取文件名 "picture_1.bmp"
	FileName = FileName.Left(FileName.ReverseFind('.')); //去除后缀 "picture_1"
	FileSuffix = picpath.Right(picpath.GetLength() - picpath.ReverseFind('.')); //获取文件的后缀 ".bmp"

	PicSavePath = savepath + "\\" + FileName + "-color" + FileSuffix;  //"...\\picture_1-color.bmp"
	ExcelSavePath = savepath + "\\" + FileName;
	ExcelSavePath += ".xls";   //"...\\picture_1.xls"
}


//总处理
//void Pictrue::Progressing()
//{
//	int path_size = vecPicPaths.size();
//	for (int num = 0; num < path_size; ++num)
//	{
//		CString picpath = vecPicPaths[num];
//		USES_CONVERSION;
//		cv::String picpath_cv = W2A(picpath);
//		Mat srcImg = imread(picpath_cv, 0);
//
//		SetFilePath(picpath, savePath);   //得到文件名和文件后缀
//		Initialize();   //每次先初始化参数
//		ComputeAccuracy(srcImg);   // 保存结果图并精确计算出各项参数
//		WriteResultToExcelAccuracy();   // 将结果保存到excel文件
//	}
//	AfxMessageBox(_T("处理完成！"));
//}