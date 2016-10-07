// Test0Dlg.h : header file
//

#if !defined(AFX_TEST0DLG_H__E3835BFA_43DF_4093_906A_8CE88E2601C6__INCLUDED_)
#define AFX_TEST0DLG_H__E3835BFA_43DF_4093_906A_8CE88E2601C6__INCLUDED_

#include "Debugger.h"	// Added by ClassView
#include "AppDev.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTest0Dlg dialog

class CTest0Dlg : public CDialog
{
// Construction
public:
	CTest0Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTest0Dlg)
	enum { IDD = IDD_TEST0_DIALOG };
	CTabCtrl	m_MainMenu;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTest0Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTest0Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CAppDev m_MenuAppDev;
	CDebugger m_MenuDebugger;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEST0DLG_H__E3835BFA_43DF_4093_906A_8CE88E2601C6__INCLUDED_)
