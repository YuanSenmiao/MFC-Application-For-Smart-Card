#if !defined(AFX_APPDEV_H__F5C143CF_FB49_46BF_87EC_0F9A8CD75C80__INCLUDED_)
#define AFX_APPDEV_H__F5C143CF_FB49_46BF_87EC_0F9A8CD75C80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AppDev.h : header file
//
#include "AdoMySQLHelper.h"
/////////////////////////////////////////////////////////////////////////////
// CAppDev dialog

class CAppDev : public CDialog
{
// Construction
public:
	int countSec;
	bool getUID();
	CString History;
	
	void updateHistory(CString,CString,long);
	void displayAccount(long, int);

	bool isValiadInput(CString str); //检查输入是否合法(数字)
	int getRemainTimeSeconds(CString h,CString m,CString s);//获取余时秒数
	
	int page;
	int block;
	unsigned char pswtype;
	unsigned char psw[16];
	CString context;
	
	CString time;
	CString mBalances;
	CString _status;

	CAdoMySQLHelper dbHelper;
	CString UID;
	
	void setRemainTimeBox(int rm);
	void emptyRemainTimeBox();

	/******************************************/
	CAppDev(CWnd* pParent = NULL);   // standard constructor
	~CAppDev(){KillTimer(2);}; 
// Dialog Data
	//{{AFX_DATA(CAppDev)
	enum { IDD = IDD_APPDEV };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppDev)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAppDev)
	afx_msg void OnInitBalance();
	afx_msg void OnCheckBalance();
	afx_msg void OnDeposit();
	afx_msg void OnConsume();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnButtonNetworkRegister();
	afx_msg void OnButtonNetworkStart();
	afx_msg void OnButtonNetworkExit();
	afx_msg void OnButtonTimeInit();
	afx_msg void OnButtonTimeDefault();
	afx_msg void OnButtonTimeCheck();
	afx_msg void OnButtonHistoryClear();
	afx_msg void OnButtonHistoryLoad();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool is_failed;
	bool m_ValiadBalance;/* 用于标识余额是否合法 */

	/** new ***************************************************************/
	long account;
	int currentUserTime;
	bool isShowTimeChange;
	bool isConnect;
	bool isOnline;
	int startTime;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPDEV_H__F5C143CF_FB49_46BF_87EC_0F9A8CD75C80__INCLUDED_)
