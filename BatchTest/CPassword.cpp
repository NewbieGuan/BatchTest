// CPassword.cpp: 实现文件
//

//#include "pch.h"
#include "stdafx.h"
#include "BatchTest.h"
#include "CPassword.h"
#include "afxdialogex.h"


// CPassword 对话框

IMPLEMENT_DYNAMIC(CPassword, CDialogEx)

CPassword::CPassword(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PASSWORD, pParent)
{

}


CPassword::~CPassword()
{
}

void CPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPassword, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPassword::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPassword::OnBnClickedCancel)
END_MESSAGE_MAP()


// CPassword 消息处理程序

// 计算正确的密码
string CPassword::GetCorrectPassword() {
	// 1.计算过了多少个半年
	CTime t1(2020, 6, 1, 0, 0, 0); // 设定起始日期为 2020.6.1 00:00
	CTime t2;
	t2 = CTime::GetCurrentTime(); //获取当前时间
	CTimeSpan ts = t2 - t1;     // Subtract 2 CTimes
	int days = ts.GetDays();   //获得相差天数
	if (days < 0) days = -days;
	int key = days / (30 * 6);  //每半年密码改一次

	// 2.生成正确的密码
	string pw_correct;
	char letters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
		'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
		'y', 'z' };
	unsigned length = sizeof(letters) / sizeof(letters[0]);
	unsigned k = key + 10;
	for (int i = 0; i < 8; ++i)
	{
		k = k * 3 % 1024;
		pw_correct.push_back(letters[(k%length)]);
	}
	return pw_correct;
}


// 判断密码是否正确
bool CPassword::is_correct() {
	// 1.读取password.ini文件中的密码
	ifstream input;
	string pw_file;
	input.exceptions(std::ifstream::failbit | std::ifstream::badbit);// 这句代码的意思是可以保证输入流对象可以正常抛出异常
	try {
		input.open(".//BatchTestIni//password.ini");
		getline(input, pw_file);
		input.close();
	}
	catch (std::ifstream::failure error)
	{
		AfxMessageBox(_T("缺少配置文件！"));
		return false;
	}

	// 2.获得正确密码
	string pw_correct = GetCorrectPassword();

	// 3.判断password.ini文件中的密码是否正确
	if (pw_file == pw_correct)
		return true;
	else
		return false;
}


// 按下OK按钮
void CPassword::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString pw_input;
	GetDlgItem(IDC_EDIT_PW)->GetWindowText(pw_input);
	string str_pw = GetCorrectPassword();
	CString pw_correct(str_pw.c_str());
	if (pw_input == pw_correct) {
		// 将正确密码写入password.ini文件
		ofstream output;
		output.exceptions(std::ofstream::failbit | std::ofstream::badbit);// 这句代码的意思是可以保证输出流对象可以正常抛出异常
		try {
			output.open(".//BatchTestIni//password.ini");
			output << str_pw;
			output.close();
		}
		catch (std::ifstream::failure error)
		{
			AfxMessageBox(_T("缺少配置文件！"));
			return;
		}
		CDialogEx::OnOK();
	}
	else
		AfxMessageBox(_T("密码错误！"));
}


// 按下Cancel按钮
void CPassword::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
