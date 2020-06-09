
// BatchTestDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "BatchTest.h"
#include "BatchTestDlg.h"
#include "afxdialogex.h"

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
	, m_resolution(5.378)
	, m_diaThreshold(150)
	, m_grayThreshold(20)
	, m_savePath(_T(""))
	, m_perimeterThreshold(50000)
	, m_radio(6)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBatchTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, EDIT_RESOLUTION, m_resolution);
	DDX_Text(pDX, EDIT_DIATHRESHOLD, m_diaThreshold);
	DDV_MinMaxInt(pDX, m_diaThreshold, 0, INT_MAX);
	DDX_Text(pDX, EDIT_GRAYTHRESHOLD, m_grayThreshold);
	DDX_Text(pDX, EDIT_SAVEPATH, m_savePath);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Text(pDX, IDC_EDIT1, m_perimeterThreshold);
	DDX_Text(pDX, IDC_EDIT2, m_radio);
}

BEGIN_MESSAGE_MAP(CBatchTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BT_SELECT_PICS, &CBatchTestDlg::OnBnClickedBt_SelectPics)
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

	m_progress.SetRange32(0, 1000);   //进度条范围

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



//按下“选择图片文件”按钮
void CBatchTestDlg::OnBnClickedBt_SelectPics()
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
	UpdateData(TRUE);
	m_savePath = ShowDialog();
	UpdateData(FALSE);
}


//按下“处理图片”按钮
void CBatchTestDlg::OnBnClickedProcessingPic()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(BT_PROCESSING_PIC)->EnableWindow(FALSE);//让处理图片按钮暂时不能点击
	UpdateData(TRUE);
	if (vecPicPaths.empty())  //如果图片路径为空
		AfxMessageBox(_T("请选择图片！"));
	else if(!PathFileExists(m_savePath))  //如果结果保存路径为空
		AfxMessageBox(_T("请选择处理结果保存路径！"));
	else
	{
		pic.SetParameter(m_resolution, m_diaThreshold, m_grayThreshold, m_perimeterThreshold, m_radio,m_savePath, vecPicPaths);  //传递参数
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