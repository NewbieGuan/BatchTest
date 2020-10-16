#pragma once

#include <string>
#include <fstream>
using namespace std;

// CPassword 对话框

class CPassword : public CDialogEx
{
	DECLARE_DYNAMIC(CPassword)

public:
	CPassword(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CPassword();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PASSWORD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	string GetCorrectPassword();   // 计算正确的密码
	bool is_correct();    // 判断密码是否正确
	afx_msg void OnBnClickedOk();   // 按下OK按钮
	afx_msg void OnBnClickedCancel();   // 按下Cancel按钮
};
