// Test0.h : main header file for the TEST0 application
//

#if !defined(AFX_TEST0_H__3EDCD069_69DD_4474_9C3D_71E1A6FD0AA4__INCLUDED_)
#define AFX_TEST0_H__3EDCD069_69DD_4474_9C3D_71E1A6FD0AA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTest0App:
// See Test0.cpp for the implementation of this class
//

class CTest0App : public CWinApp
{
public:
	CTest0App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTest0App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTest0App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEST0_H__3EDCD069_69DD_4474_9C3D_71E1A6FD0AA4__INCLUDED_)
