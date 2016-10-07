// AppDev.cpp : implementation file
//

#include "stdafx.h"
#include "Test0.h"
#include "AppDev.h"
#include "Debugger.h"
#include "string.h"
#include "RecordHelper.h"

#pragma comment(lib,"./libs/ZM124U.lib")
#include "./libs/ZM124U.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RemainTimeInit 7200
/////////////////////////////////////////////////////////////////////////////
// CAppDev dialog


CAppDev::CAppDev(CWnd* pParent /*=NULL*/)
: CDialog(CAppDev::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAppDev)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	countSec = 0;
	page = 15;
	block = 0;
	pswtype = 0x0A;
	_status = _T("");
	mBalances = _T("");
	context = _T("");
	time = _T("");
	History = _T("HISTORY");
	UID = _T("");
	isConnect = false;
	isShowTimeChange = false;
	isOnline = false;
	currentUserTime = 0;
	startTime = 0;	
}


void CAppDev::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppDev)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAppDev, CDialog)
//{{AFX_MSG_MAP(CAppDev)
ON_BN_CLICKED(IDC_INIT_BALANCE, OnInitBalance)
ON_BN_CLICKED(IDC_CHECK_BALANCE, OnCheckBalance)
ON_BN_CLICKED(IDC_DEPOSIT, OnDeposit)
ON_BN_CLICKED(IDC_CONSUME, OnConsume)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_NETWORK_REGISTER, OnButtonNetworkRegister)
	ON_BN_CLICKED(IDC_BUTTON_NETWORK_START, OnButtonNetworkStart)
	ON_BN_CLICKED(IDC_BUTTON_NETWORK_EXIT, OnButtonNetworkExit)
	ON_BN_CLICKED(IDC_BUTTON_TIME_INIT, OnButtonTimeInit)
	ON_BN_CLICKED(IDC_BUTTON_TIME_DEFAULT, OnButtonTimeDefault)
	ON_BN_CLICKED(IDC_BUTTON_TIME_CHECK, OnButtonTimeCheck)
	ON_BN_CLICKED(IDC_BUTTON_HISTORY_CLEAR, OnButtonHistoryClear)
	ON_BN_CLICKED(IDC_BUTTON_HISTORY_LOAD, OnButtonHistoryLoad)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppDev message handlers
/*	
	电子钱包初始化按钮点击响应函数，
	用户在“余额”的编辑框中输入金额，
	点击钱包初始化，将钱包设置为对应的金额。
*/
void CAppDev::OnInitBalance() 
{
	//getUID();

	// TODO: Add your control notification handler code here
	CString balance;	//电子钱包余额
	is_failed = true;
	((CEdit*)GetDlgItem(IDC_BALANCE))->GetWindowText(balance); //从编辑框获取输入的金额数 
	if(balance.IsEmpty()){//输入为空
		is_failed = true;
		this->m_ValiadBalance = false;//金额设置不合法
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Amount input is empty!"); //状态编辑框显示提醒
		return;
	}
	// 初始化金额允许为负数，故不处理其他情况。

	/*	1. 获取编辑框中输入的金额，得到的是CString类型，
		write_account函数的金额参数须为LONG类型，
		要将金额转换为long类型，使用atol()函数；*/
	account = atol(balance);	// account：初始化的数据

	/*	2. 设置使用write_account函数所需要的参数，
		对钱包的操作使用默认密钥“FFFFFFFFFFFF”，
		密钥类型“0x0A”，扇区“15”和块号“0” 
	*/
	int page=15,block=0;		// page：M1卡扇区号,block：M1卡块号 
	unsigned char pswtype=0x0A; // pswtype：密钥类型 
	CString pswCStr="FFFFFFFFFFFF"; // pswCStr：16进制CString密钥值，需要转化为字节流-> psw
	unsigned char psw[10];		// psw: 密钥值
	int asc_len;
	
	//	3. 类型转换：16进制CString密钥值转化为unsigned char*psw
	CDebugger::HexStr2CharArray(pswCStr,psw,&asc_len);

	/*	4. 函数write_account()用于向卡片钱包中写入金额，
		写入后，将成功或失败的结果显示在“状态”编辑框中。*/
	if(IFD_OK==(write_account(page, block, pswtype, psw, account))){
		is_failed = false;
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Initialize E-Wallet successfully!");

		//////////
		displayAccount(account,balance.GetLength());
		CRecordHelper myrecord;
		myrecord.walletRecord(UID,balance,account,"Initilaize");
		OnButtonHistoryLoad();

	}
	else{//写入账户余额失败
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Initialize E-Wallet failed!");
	}
	((CEdit*)GetDlgItem(IDC_BALANCE))->SetWindowText("");
}

/////////////////////////////////////////////////////////////////////////////
/*	
	余额查询按钮点击响应函数，
	使用read_account函数获得账户余额，并显示在“余额”编辑框中。
*/
void CAppDev::OnCheckBalance() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	/*	1. 设置使用read_account函数所需要的参数，
		对钱包的操作使用默认密钥“FFFFFFFFFFFF”，
		密钥类型“0x0A”，扇区“15”和块号“0” 
	*/
	int page=15,block=0;		// page：M1卡扇区号,block：M1卡块号 
	unsigned char pswtype=0x0A; // pswtype：密钥类型 
	CString pswCStr="FFFFFFFFFFFF"; // pswCStr：16进制CString密钥值，需要转化为字节流-> psw
	unsigned char psw[10];		// psw: 密钥值
	int asc_len;
	// 类型转换：16进制CString密钥值转化为unsigned char*psw
	CDebugger::HexStr2CharArray(pswCStr,psw,&asc_len);

	long account;		// long类型账户余额
	if(IFD_OK==read_account(page, block, pswtype, psw, &account)){	
		CString balance;	// CString类型账户余额	
		// read_account函数赋值的账户余额account，类型是long，需转换为CString类型
		balance.Format("%ld",account);
		((CEdit*)GetDlgItem(IDC_BALANCE))->SetWindowText(balance);
		is_failed = false;
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Check balance successfully!");
	}
	else{//读取账户余额失败
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Check balance failed!");
		((CEdit*)GetDlgItem(IDC_BALANCE))->SetWindowText("");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
/*	
	余额充值按钮点击响应函数，
	调用read_account获取原有余额，加上充值的余额后，再调用write_account写回账户中。
	需要添加输入合法性和账户金额合法性的判断。
	注意获取数据和函数参数类型。
*/
void CAppDev::OnDeposit() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	/*	1. 设置使用read_account函数所需要的参数，
		对钱包的操作使用默认密钥“FFFFFFFFFFFF”，
		密钥类型“0x0A”，扇区“15”和块号“0” 
	*/
	int page=15,block=0;		// page：M1卡扇区号,block：M1卡块号 
	unsigned char pswtype=0x0A; // pswtype：密钥类型 
	CString pswCStr="FFFFFFFFFFFF"; // pswCStr：16进制CString密钥值，需要转化为字节流-> psw
	unsigned char psw[10];		// psw: 密钥值
	int asc_len;
	// 2. 类型转换：16进制CString密钥值转化为unsigned char*psw
	CDebugger::HexStr2CharArray(pswCStr,psw,&asc_len);
	// 3. read_account读取原来账户中的余额
	long  srcAccount,desAccount,deposit;
	CString srcAccountStr,desAccountStr,depositStr;

	if(IFD_OK==read_account(page, block, pswtype, psw, &srcAccount)){
		
		// 4. 获取编辑框输入的充值金额depositStr
		((CEdit*)GetDlgItem(IDC_DEPOSIT_SUM))->GetWindowText(depositStr);

		if(depositStr.IsEmpty()){//未输入金额，设置状态编辑框，并将m_ValiadBalance变量设为false
			is_failed = true;
			this->m_ValiadBalance = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Amount input is empty!");	
			return;
		}

		// 5. 获取数据后，将CSting 类型的depositStr转成long类型的deposit
		deposit = atol(depositStr);
		if(deposit<=0){//输入金额<=0，设置状态编辑框，并将m_ValiadBalance变量设为false
			is_failed = true;
			this->m_ValiadBalance = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Illegal input!");	
			return;
		}
		// 6. 充值后余额=原有余额+充值金额
		desAccount = srcAccount+deposit;
		// 7. 调用write_account初始化余额
		if(IFD_OK==(write_account(page, block, pswtype, psw, desAccount))){
			// 7. read_account函数赋值的账户余额account，类型是long，需转换为CString类型
			desAccountStr.Format("%ld",desAccount);
			((CEdit*)GetDlgItem(IDC_BALANCE))->SetWindowText(desAccountStr);//显示充值后的总余额
			is_failed = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Deposit successfully!");//“状态”显示

			//////////////////////////////
			AfxMessageBox("充值成功");
			account = desAccount;
			displayAccount(account,desAccountStr.GetLength());
			CRecordHelper myrecord;
			CString tmp = depositStr;
			myrecord.walletRecord(UID,tmp,account,"充值"); // 记录充值金额
			OnButtonHistoryLoad();
			
		}
		else{//写入账户余额失败
			is_failed = true;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Deposit failed!");//“状态”显示
		}
		((CEdit*)GetDlgItem(IDC_DEPOSIT_SUM))->SetWindowText("");
	}
	else{// 读取账户余额失败
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Failed to get the account!");//“状态”显示
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////
/*	
	余额消费按钮点击响应函数，
	调用read_account获取原有余额，减去消费余额后，再调用write_account写回账户中。
	需要添加输入合法性和账户金额合法性的判断。
	注意获取数据和函数参数类型。
*/
void CAppDev::OnConsume() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	/*	1. 设置使用read_account函数所需要的参数，
		对钱包的操作使用默认密钥“FFFFFFFFFFFF”，
		密钥类型“0x0A”，扇区“15”和块号“0” 
	*/
	int page=15,block=0;		// page：M1卡扇区号,block：M1卡块号 
	unsigned char pswtype=0x0A; // pswtype：密钥类型 
	CString pswCStr="FFFFFFFFFFFF"; // pswCStr：16进制CString密钥值，需要转化为字节流-> psw
	unsigned char psw[10];		// psw: 密钥值
	int asc_len;
	// 2. 类型转换：16进制CString密钥值转化为unsigned char*psw
	CDebugger::HexStr2CharArray(pswCStr,psw,&asc_len);
	
	long srcAccount,desAccount, consume;
	CString srcAccountStr,desAccountStr,consumeStr;
	// 3. read_account读取原来账户中的余额
	if(IFD_OK==read_account(page, block, pswtype, psw, &srcAccount)){
		
		// 4. 获取编辑框输入的消费金额consumeStr
		((CEdit*)GetDlgItem(IDC_CONSUME_SUM))->GetWindowText(consumeStr);
		if(consumeStr.IsEmpty()){//未输入金额，设置状态编辑框，并将m_ValiadBalance变量设为false
			is_failed = true;
			this->m_ValiadBalance = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Amount input is empty!"); //“状态”显示 
			return;
		}
		consume= atol(consumeStr);	 //将CSting 转成 long 类型
		if(consume<=0){//输入金额<=0，设置状态编辑框，并将m_ValiadBalance变量设为false
			is_failed = true;
			this->m_ValiadBalance = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Illegal input!");//“状态”显示  
			return;
		}
		else if(consume>srcAccount){//消费金额>当前余额，设置状态编辑框，并将m_ValiadBalance变量设为false
			is_failed = true;
			this->m_ValiadBalance = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Insufficient balance!");  //“状态”显示
			return;
		}
		// 消费后余额=原有余额-消费金额
		desAccount = srcAccount-consume;
		if(IFD_OK==(write_account(page, block, pswtype, psw, desAccount))){ 		
			desAccountStr.Format("%ld",desAccount);    // long 转成 Cstirng类型
			((CEdit*)GetDlgItem(IDC_BALANCE))->SetWindowText(desAccountStr);//显示消费后的总余额
			is_failed = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Consumed successfully!");//“状态”显示
		
			//////////////////////////////////
			account = desAccount;
			CRecordHelper myrecord;
			CString tmp = consumeStr;
			myrecord.walletRecord(UID,tmp,account,"消费");
			OnButtonHistoryLoad();
		}
		else{//写入账户余额失败
			is_failed = true;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Failed to consume!");//“状态”显示
		}
	}
	else{//获取账户余额失败
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Failed to get the account!");//“状态”显示
	}
}


HBRUSH CAppDev::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if(pWnd->GetDlgCtrlID() == IDC_BALANCE_STATUS)	
	{
		if(!is_failed)
			pDC->SetTextColor(RGB(0,0,255));
		else
			pDC->SetTextColor(RGB(255,0,0));
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

/********************************************************************/
/*****			New Code From Here			*************************/
/********************************************************************/

void CAppDev::OnButtonNetworkRegister() 
{
	// TODO: Add your control notification handler code here
	if(!isConnect){
		if(!dbHelper.MySQL_Connect()){
			isConnect = false;
			AfxMessageBox("Cannot link to database.");//无法连接数据库
			return;
		}	
		
		dbHelper.MySQL_ScanOnTable();		
		SetTimer( 2, 1000,NULL);//设置定时器
		
		isConnect = true;
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Success: Connect with server!");
		
	}else{
		dbHelper.MySQL_Close();
		emptyRemainTimeBox();
		isConnect = false;
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Success: Disconnect with server!");
	}
}

void CAppDev::OnButtonNetworkStart() 
{
	// TODO: Add your control notification handler code here
	if(!getUID()){
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Failed to identify UID.");
		AfxMessageBox("Failed to identify UID.");
		return;
	}

	if(!isConnect){
		AfxMessageBox("Cannot link to database.");
		return;
	}

	int rm;
	OnRecord or(UID,0,"",true);
	
	//用户正在上机：操作失败
	if( !dbHelper.MySQL_Query(UID,or) == false ){
		AfxMessageBox("You have already signed in!");
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("User has already signed in.");
		return;
	}
	//用户存在RemainTime中且不在上机：获取余时信息
	else if( !dbHelper.MySQL_Query(UID,rm) == false ){
		currentUserTime = rm; //获取剩余时间
		startTime = currentUserTime;
		
		//获取当前系统时间
		CTime time = CTime::GetCurrentTime();
		//转换格式
		CString startTime_str = time.Format("%Y/%m/%d %H:%M:%S"); 

		struct OnRecord or_tmp(UID , rm, startTime_str ,false); 

		dbHelper.MySQL_Insert(or_tmp);

		CRecordHelper myrecord;
		myrecord.SignInRecord(UID,rm); //记录到历史
		CString pCStr = "User " + UID;
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText(pCStr + " sign in successfully!");
		
		isOnline = true;  //进入上机状态
		isShowTimeChange = true;   //显示时间流逝
		setRemainTimeBox(currentUserTime);	//设置时间
 
	}
	//用户不存在，创建新对象
	else{
		//获取当前系统时间
		CTime time = CTime::GetCurrentTime();
		//转换格式
		CString startTime_str = time.Format("%Y/%m/%d %H:%M:%S"); 
		
		//初始化剩余时间
		currentUserTime = RemainTimeInit;
		startTime = currentUserTime;
		
		//创建插入数据库的新对象
		struct OnRecord or_tmp(UID , RemainTimeInit, startTime_str ,false); 
		struct RemainTime rm_tmp(UID,RemainTimeInit);

		if( !dbHelper.MySQL_Insert( rm_tmp) == true ){
			AfxMessageBox("New user sign up failure.");
		}else if(!dbHelper.MySQL_Insert(or_tmp) == true){
			AfxMessageBox("New user sign in failure.");		
		}
		else{
			CRecordHelper myrecord;
			myrecord.SignInRecord(UID,RemainTimeInit); //记录到历史
			((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText(" Sign up success! Status: online");

			isOnline = true; //进入上机状态
			isShowTimeChange = true;
			setRemainTimeBox(currentUserTime);
		}
	}
}

void CAppDev::OnButtonNetworkExit() 
{
	// TODO: Add your control notification handler code here
	if(!getUID()){
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Failed to identify UID.");
		AfxMessageBox("Failed to identify UID.");
		return;
	}

	if(!isConnect){
		AfxMessageBox("Disconected with server.");
		return;
	}
	
	//查找OnRecord表
	struct OnRecord or;
	if( !dbHelper.MySQL_Query(UID,or) == false ){  //使用否定判断
		// ((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("A");
		//更新RemainTimeTable中的剩余时间值
		if( !dbHelper.MySQL_UpdateRemainTime(UID, or.RemainSeconds,"RemainTimeTable")){
			((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Failed to update user remain time.");
			return;
		}
		
		if(!dbHelper.MySQL_Delete(UID,"onlinetable")){
			is_failed = true;
			((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Failed to exit.");
			is_failed = false;
			return;
		}

		CRecordHelper myrecord;
		myrecord.ExitRecord(UID,currentUserTime,startTime-currentUserTime); //记录到历史框
		
		emptyRemainTimeBox();  //清空对话框
		CString pCStr = "User " + UID;
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText(pCStr+" Exit successfully!");
		isOnline = false;  //取消上机状态
		isShowTimeChange = false;
		UID = ""; //清除全局变量UID

	}else {
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Exit failure. User is offline.");
		emptyRemainTimeBox();  //清空对话框
		is_failed = false;
	}
}

void CAppDev::OnButtonTimeInit() 
{
	// TODO: Add your control notification handler code here
	if(!getUID()){
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Failed to identify UID.");
		AfxMessageBox("Failed to identify UID.");
		return;
	}

	if(!isConnect){
		AfxMessageBox("Disconnected with server.");
		return;
	}
	
	CString hour_str;
	((CEdit*)GetDlgItem(IDC_EDIT_HOUR))->GetWindowText(hour_str);
	if(!isValiadInput(hour_str) ){
		AfxMessageBox("Wrong time format [hour]!");
		return;
	}

	CString min_str;
	((CEdit*)GetDlgItem(IDC_EDIT_MINUTE))->GetWindowText(min_str);
	if(!isValiadInput(min_str) || atoi(min_str) >= 60){
		AfxMessageBox("Wrong time format! [minute]");
		return;
	}


	CString sec_str;
	((CEdit*)GetDlgItem(IDC_EDIT_SECOND))->GetWindowText(sec_str);
	if(!isValiadInput(sec_str) || atoi(sec_str) >= 60){
		AfxMessageBox("Wrong time format! [second]");
		return;
	}

	int remainSeconds = getRemainTimeSeconds(hour_str,min_str,sec_str);
	currentUserTime = remainSeconds;

	int rm;
	struct OnRecord or;
	
	if( !dbHelper.MySQL_Query(UID,or) == false ){ 
		if( !dbHelper.MySQL_UpdateRemainTime(UID,remainSeconds,"onlinetable")){
			((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("failed to set remain time");
			return;
		}

		if( !dbHelper.MySQL_UpdateRemainTime(UID,remainSeconds,"RemainTimeTable")){
			((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("failed to set remain time");
			return;
		}
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Init remain time successfully");

	}
	else if( !dbHelper.MySQL_Query(UID,rm) == false ){
		if( !dbHelper.MySQL_UpdateRemainTime(UID,remainSeconds,"RemainTimeTable")){
			((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("failed to set remain time");
			return;
		}
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Set remain time successfully");
	}
	else{
		AfxMessageBox("Invaliad request. Non-exist user.");
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Invaliad request. Non-exist user.");
	}

	setRemainTimeBox(remainSeconds);
}

//获取剩余秒数
int CAppDev::getRemainTimeSeconds(CString h,CString m,CString s){
	return atoi(h)*3600+atoi(m)*60+atoi(s)*1;
}

//检查输入是否为纯数字
bool CAppDev::isValiadInput(CString str){
	for(int ii=0; ii<str.GetLength(); ii++){
		if(str[ii]>='0' && str[ii] <= '9')
			continue;
		else{
			return false;
		}
	}
	return true;

}

void CAppDev::OnButtonTimeDefault() 
{
	// TODO: Add your control notification handler code here
	if(!getUID()){
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Failed to identify UID.");
		AfxMessageBox("Failed to identify UID.");
		return;
	}

	if(!isConnect){
		AfxMessageBox("Disconnected with server.");
		return;
	}

	int remainTime;
	struct OnRecord or;

	if( !dbHelper.MySQL_Query(UID,or) == false ){ //同时存在于上机表和余时表中
		if(dbHelper.MySQL_UpdateRemainTime(UID,RemainTimeInit,"onlinetable") && dbHelper.MySQL_UpdateRemainTime(UID,RemainTimeInit,"RemainTimeTable")){
			//不显示时间流逝
			isShowTimeChange = false;
			setRemainTimeBox(RemainTimeInit);
			((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("[Online] Set default time successfully.");
			currentUserTime = RemainTimeInit;
		}else{
			AfxMessageBox("[Online] Set default time failure");
		}
	}
	else if( !dbHelper.MySQL_Query(UID,remainTime) == false){	//只存在余时表中
		if(dbHelper.MySQL_UpdateRemainTime(UID,RemainTimeInit,"RemainTimeTable")){
			
			isShowTimeChange = false;
			setRemainTimeBox(RemainTimeInit);
			((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("[Offline] Set default time successfully.");
			currentUserTime = RemainTimeInit;
		}else{
			AfxMessageBox("[Offline] Set default time failure");
		}
	} else{ 
		AfxMessageBox("Invaliad request. Non-exist user.");
		currentUserTime = 0;
	}
}

void CAppDev::OnButtonTimeCheck() 
{
	// TODO: Add your control notification handler code here
	if(!getUID()){
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Failed to identify UID.");
		AfxMessageBox("Failed to identify UID.");
		return;
	}

	if(!isConnect){
		AfxMessageBox("Disconnected with server.");
		return;
	}

	//查询到余时数据
	int remainTime;
	struct OnRecord or;

	if( !dbHelper.MySQL_Query(UID,or) == false ){
		//显示时间流逝
		isShowTimeChange = true;  //显示时间流逝
		currentUserTime = or.RemainSeconds;		
		setRemainTimeBox(currentUserTime);
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Check time successfully"); 
	}else if(!dbHelper.MySQL_Query(UID,remainTime) == false ){
		isShowTimeChange = true; //显示时间流逝
		currentUserTime = remainTime;		
		setRemainTimeBox(currentUserTime);		
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Check time successfully");
	}else{
		AfxMessageBox("Invaliad request. Non-exist user.");
		currentUserTime = 0;
	}
}

void CAppDev::OnButtonHistoryClear() 
{
	// TODO: Add your control notification handler code here
	CRecordHelper mrecord;
	if(mrecord.isEmptyRecord())
		AfxMessageBox("History cleared!");
	((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->SetWindowText("");
}

void CAppDev::OnButtonHistoryLoad() 
{
	// TODO: Add your control notification handler code here
	CRecordHelper mrecord;
	CString str = mrecord.LoadRecords();
	((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->SetWindowText(str);
}

void CAppDev::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	//AfxMessageBox("定时器");	
	if(nIDEvent == 2 && isConnect ){
		countSec = countSec+1;
		if(countSec >= 5){
			countSec = 0;
			if( !dbHelper.MySQL_ScanOnTable()){
				return;
			}						
		}		
		if(isOnline){
			if(currentUserTime > 0){
				currentUserTime--;
			}
		}
		if(isShowTimeChange && isOnline){
			setRemainTimeBox(currentUserTime);
		}else{/*
			((CEdit*)GetDlgItem(IDC_EDIT_HOUR))->SetWindowText("");
			((CEdit*)GetDlgItem(IDC_EDIT_MINUTE))->SetWindowText("");
			((CEdit*)GetDlgItem(IDC_EDIT_SECOND))->SetWindowText("");*/
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

////////////////////////////////////////

bool CAppDev::getUID(){
	/*
	CString uidCStr, tmp;
	unsigned char uid[100]; //	uid：获取的UID信息
	int uid_len;			//	uid_len：信息的长度
	// 1. 获取卡号UID
	is_failed = true;
	if (IFD_OK == find_14443(uid,&uid_len)){
		is_failed = false;
	// 2. 类型转换
		// 将unsigned char*类型的uid转换为CString类型的uidCStr
		   用循环拼接得到CString 
		uidCStr.Empty();
		for (int i=0;i<uid_len;i++)
		{
			tmp.Format("%02X",uid[i]);	// 将获取的UID数据(1字节)转为16进制，02x 表示2位大写的16进制
			uidCStr += tmp; 			// CString拼接
		}
	// 3. 编辑框显示卡号UID，并更新状态编辑框，获取卡号成功
		((CEdit*)GetDlgItem(IDC_UID))->SetWindowText(uidCStr);
		((CEdit*)GetDlgItem(IDC_INFO_STATUS))->SetWindowText("Get UID successfully");
		return true;
	}else{	// 获取失败
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_UID))->SetWindowText("");
		((CEdit*)GetDlgItem(IDC_INFO_STATUS))->SetWindowText("Get UID failed");
		return false;
	}
	*/
	// test

	UID = "8c742180";
	return true;

}

void CAppDev::displayAccount(long account, int len)
{
	char *data = new char[len];
	memset(data,'\0',len);
	CString s;
	s.Format("%ld",account);
	strncpy(data, (LPCTSTR)s, sizeof(data));
	LED(data, len, 0);
}

void CAppDev::updateHistory(CString _Status, CString _Content, long ACCOUNT)
{
	
	CString tmp;
	unsigned char buff[5];
	int buff_len;
	// 获取卡号
	
	if(IFD_OK == find_14443(buff,&buff_len)){
		CString a;
		a.Format("%d",buff_len);
		//UID.Empty();
		for(int i=0;i<buff_len;i++){
			tmp.Format("%02x",buff[i]); //将获取的uid转换成16进制保存在CString类型的变量中
			UID = UID + tmp;
			MessageBox(tmp);
		}
		MessageBox(a);
	}
	else{
		AfxMessageBox("Failed to identify UID.");
		return;
	}
	MessageBox("##");
	// 获取时间
	unsigned char TIME[16] = {0};
	int TIME_LEN = 0;
	int x = Get_time(TIME,&TIME_LEN);
	

	CString temp = _T("");
	for(int i=0; i<TIME_LEN; i++){
		temp.Format("%u",TIME[i]);
		MessageBox(temp);
		time += temp;
	}
	//MessageBox(time);
//	delete [] TIME;
	// 获取余额
	/*
	char xx[10] = {0};
	ltoa(ACCOUNT,xx,10);
	CString tmp2("");
	for(int ii=0; ii<10; ii++)
	{
		tmp.Format("%c",xx[ii]);
		mBalances += tmp2;
	}

	History += _T("\r\n") + UID + time + _Status + _Content + mBalances;
	*/
	((CEdit*)GetDlgItem(IDC_EDIT_HISTORY))->SetWindowText(History);
}
//更新对话框的剩余时间内容
void CAppDev::emptyRemainTimeBox(){
	((CEdit*)GetDlgItem(IDC_EDIT_HOUR))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_MINUTE))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_SECOND))->SetWindowText("");
}

//更新余时框的剩余时间内容
void CAppDev::setRemainTimeBox(int rm){
	
	int hour = rm/3600;
	int minute = (rm%3600)/60;
	int second = rm%60;

	CString tmph,tmpm,tmps;
	tmph.Format("%d",hour);
	tmpm.Format("%d",minute);
	tmps.Format("%d",second);

	((CEdit*)GetDlgItem(IDC_EDIT_HOUR))->SetWindowText(tmph);
	((CEdit*)GetDlgItem(IDC_EDIT_MINUTE))->SetWindowText(tmpm);
	((CEdit*)GetDlgItem(IDC_EDIT_SECOND))->SetWindowText(tmps);

}