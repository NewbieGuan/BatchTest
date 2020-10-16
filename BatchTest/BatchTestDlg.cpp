
// BatchTestDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "BatchTest.h"
#include "BatchTestDlg.h"
#include "afxdialogex.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBatchTestDlg 对话框



CBatchTestDlg::CBatchTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BATCHTEST_DIALOG, pParent)
	//, m_resolution(5.378)
	//, m_diaThreshold_low(150)
	//, m_diaThreshold_high(300)
	//, m_grayThreshold(20)
	//, m_perimeterThreshold(50000)
	//, m_radio(6)
	, m_savePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBatchTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, EDIT_RESOLUTION, m_resolution);
	DDX_Text(pDX, EDIT_DIATHRESHOLD_L, m_diaThreshold_low);
	DDX_Text(pDX, EDIT_DIATHRESHOLD_H, m_diaThreshold_high);
	DDV_MinMaxInt(pDX, m_diaThreshold_low, 0, INT_MAX);
	DDV_MinMaxInt(pDX, m_diaThreshold_high, 0, INT_MAX);
	DDX_Text(pDX, EDIT_GRAYTHRESHOLD, m_grayThreshold);
	DDX_Text(pDX, EDIT_SAVEPATH, m_savePath);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Text(pDX, IDC_PERIMETERTHRESHOLD, m_perimeterThreshold);
	DDX_Text(pDX, IDC_RADIO, m_radio);
}

BEGIN_MESSAGE_MAP(CBatchTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BT_SELECT_PICS, &CBatchTestDlg::OnBnClickedSelectPics)
	ON_BN_CLICKED(BT_SELECT_SAVEPATH, &CBatchTestDlg::OnBnClickedSelectSavepath)
	ON_BN_CLICKED(BT_PROCESSING_PIC, &CBatchTestDlg::OnBnClickedProcessingPic)
END_MESSAGE_MAP()


// CBatchTestDlg 消息处理程序

BOOL CBatchTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_progress.SetRange32(0, 1000);   //1.进度条范围

	PasswordCheck();   //2.密码处理

	GetIniPara();   //3.从配置文件中初始化参数

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CBatchTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBatchTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBatchTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//密码处理
void CBatchTestDlg::PasswordCheck() {
	if (dlgPassword.is_correct()) return;

	GetDlgItem(BT_SELECT_PICS)->EnableWindow(FALSE);//让选择图片文件按钮暂时不能点击
	GetDlgItem(BT_SELECT_SAVEPATH)->EnableWindow(FALSE);//让选择保存路径按钮暂时不能点击
	GetDlgItem(BT_PROCESSING_PIC)->EnableWindow(FALSE);//让处理图片按钮暂时不能点击

	dlgPassword.DoModal();
	if (dlgPassword.is_correct()) {
		GetDlgItem(BT_SELECT_PICS)->EnableWindow(TRUE);
		GetDlgItem(BT_SELECT_SAVEPATH)->EnableWindow(TRUE);
		GetDlgItem(BT_PROCESSING_PIC)->EnableWindow(TRUE);
	}
}


//按下“选择图片文件”按钮
void CBatchTestDlg::OnBnClickedSelectPics()
{
	// TODO: 在此添加控件通知处理程序代码
	vecPicPaths.clear();

	CFileDialog dlg(TRUE, _T("*.bmp"), NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("Image Files(*.bmp;*.jpg;*.png;*.tif)|*.bmp;*.jpg;*.png;*.tif||"), NULL);
	dlg.m_ofn.lpstrTitle = _T("选择图片");
	// 为了实现多文件同时添加
	DWORD max_file = 40000;             // 定义own filename buffer的大小
	TCHAR * lsf = new TCHAR[max_file];
	dlg.m_ofn.nMaxFile = max_file;
	dlg.m_ofn.lpstrFile = lsf;
	dlg.m_ofn.lpstrFile[0] = NULL;      // 初始化对话框

	CString filename = _T("");
	if (dlg.DoModal() == IDOK)
	{
		POSITION fileNamesPosition = dlg.GetStartPosition();
		while (fileNamesPosition != NULL)
		{
			filename = dlg.GetNextPathName(fileNamesPosition);
			vecPicPaths.push_back(filename);
		}
	}
	delete lsf;

	size_t count = vecPicPaths.size();
	if (count == 0)
		MessageBox(_T("没有选择文件"), NULL, MB_OK | MB_ICONQUESTION);
	else
		sort(vecPicPaths.begin(), vecPicPaths.end());   //对路径名进行字典排序，使之有序
}


//按下“选择保存路径”按钮
void CBatchTestDlg::OnBnClickedSelectSavepath()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);//把控件的内容更新到变量
	m_savePath = ShowDialog();
	UpdateData(FALSE);//把变量的内容更新到控件
}


//按下“处理图片”按钮
void CBatchTestDlg::OnBnClickedProcessingPic()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	SaveIniPara();
	if (vecPicPaths.empty())  //如果图片路径为空
		AfxMessageBox(_T("请选择图片！"));
	else if (!PathFileExists(m_savePath))  //如果结果保存路径为空
		AfxMessageBox(_T("请选择处理结果保存路径！"));
	else if (m_diaThreshold_low > m_diaThreshold_high)
		AfxMessageBox(_T("粒径筛选范围有误！"));
	else
	{
		GetDlgItem(BT_PROCESSING_PIC)->EnableWindow(FALSE);//让处理图片按钮暂时不能点击
		pic.SetParameter(m_resolution, m_diaThreshold_low, m_diaThreshold_high, m_grayThreshold, m_perimeterThreshold, m_radio,m_savePath, vecPicPaths);  //传递参数
		//AfxBeginThread(ProgressPic, NULL);  //开辟线程
		//CreateThread(0, 0, ProgressPic, &pic, 0, 0);   //开辟线程
		unsigned int threadID;
		_beginthreadex(NULL, 0, &CBatchTestDlg::ProgressPic, this, 0, &threadID);   //开辟线程
	}
}


//显示选择保存路径对话框
CString CBatchTestDlg::ShowDialog()
{
	TCHAR           szFolderPath[MAX_PATH] = { 0 };
	CString         strFolderPath = TEXT("");

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择处理结果存储路径");
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}

	return strFolderPath;
}


//线程函数
UINT CBatchTestDlg::ProgressPic(LPVOID pParam)
{
	CBatchTestDlg* dlg = static_cast<CBatchTestDlg*>(pParam);
	if (dlg)
	{
		int path_size = dlg->vecPicPaths.size();
		dlg->SetProgressBar(0, path_size);   //设置初始进度条
		dlg->SetProgressRate(0, path_size);   //设置初始进度数
		for (int num = 0; num < path_size; ++num)
		{
			CString picpath = dlg->vecPicPaths[num];
			USES_CONVERSION;
			cv::String picpath_cv = W2A(picpath);
			Mat srcImg = imread(picpath_cv, 0);

			dlg->pic.Initialize();   //每次先初始化参数
			dlg->pic.SetFilePath(picpath, dlg->pic.savePath);   //得到文件名和文件后缀
			dlg->pic.ComputeAccuracy(srcImg);   // 保存结果图并精确计算出各项参数
			dlg->pic.WriteResultToExcelAccuracy();   // 将结果保存到excel文件
			dlg->SetProgressBar(num+1, path_size);   //设置进度条
			dlg->SetProgressRate(num+1, path_size);   //设置进度数
		}
		AfxMessageBox(_T("处理完成！"));
		dlg->GetDlgItem(BT_PROCESSING_PIC)->EnableWindow(TRUE);//让处理图片回到可以点击状态
	}
	_endthreadex(0);
	return 0;
}


//设置进度条
void CBatchTestDlg::SetProgressBar(int num, int num_all)
{
	int nMin, nMax;
	m_progress.GetRange(nMin, nMax);
	int nPos = nMax * num  / num_all;
	m_progress.SetPos(nPos);
}


//设置进度数
void CBatchTestDlg::SetProgressRate(int num, int num_all)
{
	CString str;
	str.Format(_T("%d/%d"), num, num_all);
	SetDlgItemText(STATIC_RATE, str);
}


//获取配置文件参数更新到界面
void CBatchTestDlg::GetIniPara() {
	CString inipath = GetIniPath();

	// 分辨率
	CString str_resolution;
	::GetPrivateProfileString(_T("SaveInfo"), _T("分辨率"), _T("5.378"), str_resolution.GetBuffer(MAX_PATH), MAX_PATH, inipath);
	GetDlgItem(EDIT_RESOLUTION)->SetWindowText(str_resolution);
	m_resolution = _tstof(str_resolution);

	// 低粒径阈值
	CString str_diaThreshold_low;
	m_diaThreshold_low = GetPrivateProfileInt(_T("SaveInfo"), _T("低粒径阈值"), 50, inipath);
	str_diaThreshold_low.Format(_T("%d"), m_diaThreshold_low);
	GetDlgItem(EDIT_DIATHRESHOLD_L)->SetWindowText(str_diaThreshold_low);

	// 高粒径阈值
	CString str_diaThreshold_high;
	m_diaThreshold_high = GetPrivateProfileInt(_T("SaveInfo"), _T("高粒径阈值"), 1000, inipath);
	str_diaThreshold_high.Format(_T("%d"), m_diaThreshold_high);
	GetDlgItem(EDIT_DIATHRESHOLD_H)->SetWindowText(str_diaThreshold_high);

	// 灰度阈值（减数）
	CString str_grayThreshold;
	m_grayThreshold = GetPrivateProfileInt(_T("SaveInfo"), _T("灰度阈值"), 20, inipath);
	str_grayThreshold.Format(_T("%d"), m_grayThreshold);
	GetDlgItem(EDIT_GRAYTHRESHOLD)->SetWindowText(str_grayThreshold);

	// 周长阈值
	CString str_perimeterThreshold;
	m_perimeterThreshold = GetPrivateProfileInt(_T("SaveInfo"), _T("周长阈值"), 50000, inipath);
	str_perimeterThreshold.Format(_T("%d"), m_perimeterThreshold);
	GetDlgItem(IDC_PERIMETERTHRESHOLD)->SetWindowText(str_perimeterThreshold);

	// 最小外接矩形长宽比
	CString str_radio;
	m_radio = GetPrivateProfileInt(_T("SaveInfo"), _T("最小外接矩形长宽比"), 6, inipath);
	str_radio.Format(_T("%d"), m_radio);
	GetDlgItem(IDC_RADIO)->SetWindowText(str_radio);
}


//界面的参数保存到配置文件
void CBatchTestDlg::SaveIniPara() {
	CString inipath = GetIniPath();

	// 分辨率
	CString str_resolution;
	GetDlgItem(EDIT_RESOLUTION)->GetWindowText(str_resolution);
	::WritePrivateProfileString(_T("SaveInfo"), _T("分辨率"), str_resolution, inipath);

	// 低粒径阈值
	CString str_diaThreshold_low;
	GetDlgItem(EDIT_DIATHRESHOLD_L)->GetWindowText(str_diaThreshold_low);
	::WritePrivateProfileString(_T("SaveInfo"), _T("低粒径阈值"), str_diaThreshold_low, inipath);

	// 高粒径阈值
	CString str_diaThreshold_high;
	GetDlgItem(EDIT_DIATHRESHOLD_H)->GetWindowText(str_diaThreshold_high);
	::WritePrivateProfileString(_T("SaveInfo"), _T("高粒径阈值"), str_diaThreshold_high, inipath);

	// 灰度阈值（减数）
	CString str_grayThreshold;
	GetDlgItem(EDIT_GRAYTHRESHOLD)->GetWindowText(str_grayThreshold);
	::WritePrivateProfileString(_T("SaveInfo"), _T("灰度阈值"), str_grayThreshold, inipath);

	// 周长阈值
	CString str_perimeterThreshold;
	GetDlgItem(IDC_PERIMETERTHRESHOLD)->GetWindowText(str_perimeterThreshold);
	::WritePrivateProfileString(_T("SaveInfo"), _T("周长阈值"), str_perimeterThreshold, inipath);

	// 最小外接矩形长宽比
	CString str_radio;
	GetDlgItem(IDC_RADIO)->GetWindowText(str_radio);
	::WritePrivateProfileString(_T("SaveInfo"), _T("最小外接矩形长宽比"), str_radio, inipath);
}


//获得配置文件路径
CString CBatchTestDlg::GetIniPath() {
	CString exepath; //包含目录的路径
	::GetModuleFileName(NULL, exepath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	int nexe = exepath.GetLength();
	int i = nexe - 1;
	for (; i >= 0; i--)
	{
		if (exepath[i] == '\\')
			break;
	}
	CString inipath = exepath.Left(i + 1) + "BatchTestIni//para.ini"; 
	return inipath;
}