// Test0Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Test0.h"
#include "Test0Dlg.h"

#pragma comment(lib,"./libs/ZM124U.lib")
#include "./libs/ZM124U.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	/* 将变量与控件ID绑定，通过成员变量来控制控件 */
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

/* 将变量与控件ID绑定，通过成员变量来控制控件 */
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTest0Dlg dialog

CTest0Dlg::CTest0Dlg(CWnd* pParent /*=NULL*/)
: CDialog(CTest0Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTest0Dlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTest0Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTest0Dlg)
	DDX_Control(pDX, IDC_TAB1, m_MainMenu);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTest0Dlg, CDialog)
//{{AFX_MSG_MAP(CTest0Dlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTest0Dlg message handlers

BOOL CTest0Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	/* 新添加的代码 */
	/* 1.插入Tab选项卡标签  */
	/* m_MainMenu是TabControl控件对应的变量  */
	m_MainMenu.InsertItem(0,"Debug Assistant");
	m_MainMenu.InsertItem(1,"Applicant Development");
	
	/* 2.关联对话框，将TabControl控件设为选项卡对应对话框的父窗口 */
	m_MenuDebugger.Create(IDD_DEBUGGER,GetDlgItem(IDC_TAB1));
	m_MenuAppDev.Create(IDD_APPDEV,GetDlgItem(IDC_TAB1));
	
	/* 3. 获取TAB控件客户区大小，用于调整选项卡对话框在父窗口中的位置 */
	CRect rect;
    m_MainMenu.GetClientRect(&rect);
	
    /* 原点位于左上角，X轴Y轴分别指向右方和下方 */ 
	rect.top += 28;
	rect.right -= 5;
	rect.bottom -= 5;
	rect.left += 5;
	
	/* 4. 设置子对话框尺寸并移动到指定位置 */
	m_MenuDebugger.MoveWindow(&rect);
	m_MenuAppDev.MoveWindow(&rect);
	
	/* 5.设置默认选项卡，对选项卡对话框进行隐藏和显示 */
	m_MenuDebugger.ShowWindow(true);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTest0Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTest0Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTest0Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/* 处理选项卡切换  */
void CTest0Dlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
	/* 获取当前点击选项卡的标签下标，根据前面的设置
	0为调试助手标签，1为应用开发标签 */
	int cursel = m_MainMenu.GetCurSel();
	/*  根据下标将相应的对话框显示，其余隐藏，
	使用对话框对应的变量来控制 */
	switch(cursel)
	{
	case 0:/* 调试助手对应的对话框显示，其余隐藏 */
		m_MenuDebugger.ShowWindow(SW_SHOWNORMAL);
		m_MenuAppDev.ShowWindow(SW_HIDE);/*m_MenuAppDev是应用开发对话框对应的变量*/
		break;
	case 1:/* 应用开发对应的对话框显示，其余隐藏 */
		m_MenuDebugger.ShowWindow(SW_HIDE);
		m_MenuAppDev.ShowWindow(SW_SHOWNORMAL);
		break;
	default:
		break;
	}
}
