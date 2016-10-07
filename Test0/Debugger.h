#if !defined(AFX_DEBUGGER_H__E56758F6_9C15_4AE5_B08C_DDA6FD77ABA6__INCLUDED_)
#define AFX_DEBUGGER_H__E56758F6_9C15_4AE5_B08C_DDA6FD77ABA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Debugger.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDebugger dialog

class CDebugger : public CDialog
{
// Construction
public:
	CDebugger(CWnd* pParent = NULL);   // standard constructor
	static void HexStr2CharArray(CString hexStr,unsigned char *asc,int *asc_len);
// Dialog Data
	//{{AFX_DATA(CDebugger)
	enum { IDD = IDD_DEBUGGER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugger)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDebugger)
	afx_msg void OnBtnAccessUid();
	afx_msg void OnBtnOpenDevice();
	afx_msg void OnBtnCheckVersion();
	afx_msg void OnBtnSetLed();
	afx_msg void OnTypeA();
	afx_msg void OnTypeB();
	afx_msg void OnBtnDefKey();
	afx_msg void OnBtnReadSector();
	afx_msg void OnBtnReadBlock();
	afx_msg void OnBtnWriteBlock();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool is_failed;
	unsigned char m_RatioKeyType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGGER_H__E56758F6_9C15_4AE5_B08C_DDA6FD77ABA6__INCLUDED_)
