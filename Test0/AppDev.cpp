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
	����Ǯ����ʼ����ť�����Ӧ������
	�û��ڡ����ı༭���������
	���Ǯ����ʼ������Ǯ������Ϊ��Ӧ�Ľ�
*/
void CAppDev::OnInitBalance() 
{
	//getUID();

	// TODO: Add your control notification handler code here
	CString balance;	//����Ǯ�����
	is_failed = true;
	((CEdit*)GetDlgItem(IDC_BALANCE))->GetWindowText(balance); //�ӱ༭���ȡ����Ľ���� 
	if(balance.IsEmpty()){//����Ϊ��
		is_failed = true;
		this->m_ValiadBalance = false;//������ò��Ϸ�
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Amount input is empty!"); //״̬�༭����ʾ����
		return;
	}
	// ��ʼ���������Ϊ�������ʲ��������������

	/*	1. ��ȡ�༭��������Ľ��õ�����CString���ͣ�
		write_account�����Ľ�������ΪLONG���ͣ�
		Ҫ�����ת��Ϊlong���ͣ�ʹ��atol()������*/
	account = atol(balance);	// account����ʼ��������

	/*	2. ����ʹ��write_account��������Ҫ�Ĳ�����
		��Ǯ���Ĳ���ʹ��Ĭ����Կ��FFFFFFFFFFFF����
		��Կ���͡�0x0A����������15���Ϳ�š�0�� 
	*/
	int page=15,block=0;		// page��M1��������,block��M1����� 
	unsigned char pswtype=0x0A; // pswtype����Կ���� 
	CString pswCStr="FFFFFFFFFFFF"; // pswCStr��16����CString��Կֵ����Ҫת��Ϊ�ֽ���-> psw
	unsigned char psw[10];		// psw: ��Կֵ
	int asc_len;
	
	//	3. ����ת����16����CString��Կֵת��Ϊunsigned char*psw
	CDebugger::HexStr2CharArray(pswCStr,psw,&asc_len);

	/*	4. ����write_account()������ƬǮ����д���
		д��󣬽��ɹ���ʧ�ܵĽ����ʾ�ڡ�״̬���༭���С�*/
	if(IFD_OK==(write_account(page, block, pswtype, psw, account))){
		is_failed = false;
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Initialize E-Wallet successfully!");

		//////////
		displayAccount(account,balance.GetLength());
		CRecordHelper myrecord;
		myrecord.walletRecord(UID,balance,account,"Initilaize");
		OnButtonHistoryLoad();

	}
	else{//д���˻����ʧ��
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Initialize E-Wallet failed!");
	}
	((CEdit*)GetDlgItem(IDC_BALANCE))->SetWindowText("");
}

/////////////////////////////////////////////////////////////////////////////
/*	
	����ѯ��ť�����Ӧ������
	ʹ��read_account��������˻�������ʾ�ڡ����༭���С�
*/
void CAppDev::OnCheckBalance() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	/*	1. ����ʹ��read_account��������Ҫ�Ĳ�����
		��Ǯ���Ĳ���ʹ��Ĭ����Կ��FFFFFFFFFFFF����
		��Կ���͡�0x0A����������15���Ϳ�š�0�� 
	*/
	int page=15,block=0;		// page��M1��������,block��M1����� 
	unsigned char pswtype=0x0A; // pswtype����Կ���� 
	CString pswCStr="FFFFFFFFFFFF"; // pswCStr��16����CString��Կֵ����Ҫת��Ϊ�ֽ���-> psw
	unsigned char psw[10];		// psw: ��Կֵ
	int asc_len;
	// ����ת����16����CString��Կֵת��Ϊunsigned char*psw
	CDebugger::HexStr2CharArray(pswCStr,psw,&asc_len);

	long account;		// long�����˻����
	if(IFD_OK==read_account(page, block, pswtype, psw, &account)){	
		CString balance;	// CString�����˻����	
		// read_account������ֵ���˻����account��������long����ת��ΪCString����
		balance.Format("%ld",account);
		((CEdit*)GetDlgItem(IDC_BALANCE))->SetWindowText(balance);
		is_failed = false;
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Check balance successfully!");
	}
	else{//��ȡ�˻����ʧ��
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Check balance failed!");
		((CEdit*)GetDlgItem(IDC_BALANCE))->SetWindowText("");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
/*	
	����ֵ��ť�����Ӧ������
	����read_account��ȡԭ�������ϳ�ֵ�������ٵ���write_accountд���˻��С�
	��Ҫ�������Ϸ��Ժ��˻����Ϸ��Ե��жϡ�
	ע���ȡ���ݺͺ����������͡�
*/
void CAppDev::OnDeposit() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	/*	1. ����ʹ��read_account��������Ҫ�Ĳ�����
		��Ǯ���Ĳ���ʹ��Ĭ����Կ��FFFFFFFFFFFF����
		��Կ���͡�0x0A����������15���Ϳ�š�0�� 
	*/
	int page=15,block=0;		// page��M1��������,block��M1����� 
	unsigned char pswtype=0x0A; // pswtype����Կ���� 
	CString pswCStr="FFFFFFFFFFFF"; // pswCStr��16����CString��Կֵ����Ҫת��Ϊ�ֽ���-> psw
	unsigned char psw[10];		// psw: ��Կֵ
	int asc_len;
	// 2. ����ת����16����CString��Կֵת��Ϊunsigned char*psw
	CDebugger::HexStr2CharArray(pswCStr,psw,&asc_len);
	// 3. read_account��ȡԭ���˻��е����
	long  srcAccount,desAccount,deposit;
	CString srcAccountStr,desAccountStr,depositStr;

	if(IFD_OK==read_account(page, block, pswtype, psw, &srcAccount)){
		
		// 4. ��ȡ�༭������ĳ�ֵ���depositStr
		((CEdit*)GetDlgItem(IDC_DEPOSIT_SUM))->GetWindowText(depositStr);

		if(depositStr.IsEmpty()){//δ���������״̬�༭�򣬲���m_ValiadBalance������Ϊfalse
			is_failed = true;
			this->m_ValiadBalance = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Amount input is empty!");	
			return;
		}

		// 5. ��ȡ���ݺ󣬽�CSting ���͵�depositStrת��long���͵�deposit
		deposit = atol(depositStr);
		if(deposit<=0){//������<=0������״̬�༭�򣬲���m_ValiadBalance������Ϊfalse
			is_failed = true;
			this->m_ValiadBalance = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Illegal input!");	
			return;
		}
		// 6. ��ֵ�����=ԭ�����+��ֵ���
		desAccount = srcAccount+deposit;
		// 7. ����write_account��ʼ�����
		if(IFD_OK==(write_account(page, block, pswtype, psw, desAccount))){
			// 7. read_account������ֵ���˻����account��������long����ת��ΪCString����
			desAccountStr.Format("%ld",desAccount);
			((CEdit*)GetDlgItem(IDC_BALANCE))->SetWindowText(desAccountStr);//��ʾ��ֵ��������
			is_failed = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Deposit successfully!");//��״̬����ʾ

			//////////////////////////////
			AfxMessageBox("��ֵ�ɹ�");
			account = desAccount;
			displayAccount(account,desAccountStr.GetLength());
			CRecordHelper myrecord;
			CString tmp = depositStr;
			myrecord.walletRecord(UID,tmp,account,"��ֵ"); // ��¼��ֵ���
			OnButtonHistoryLoad();
			
		}
		else{//д���˻����ʧ��
			is_failed = true;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Deposit failed!");//��״̬����ʾ
		}
		((CEdit*)GetDlgItem(IDC_DEPOSIT_SUM))->SetWindowText("");
	}
	else{// ��ȡ�˻����ʧ��
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Failed to get the account!");//��״̬����ʾ
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////
/*	
	������Ѱ�ť�����Ӧ������
	����read_account��ȡԭ������ȥ���������ٵ���write_accountд���˻��С�
	��Ҫ�������Ϸ��Ժ��˻����Ϸ��Ե��жϡ�
	ע���ȡ���ݺͺ����������͡�
*/
void CAppDev::OnConsume() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	/*	1. ����ʹ��read_account��������Ҫ�Ĳ�����
		��Ǯ���Ĳ���ʹ��Ĭ����Կ��FFFFFFFFFFFF����
		��Կ���͡�0x0A����������15���Ϳ�š�0�� 
	*/
	int page=15,block=0;		// page��M1��������,block��M1����� 
	unsigned char pswtype=0x0A; // pswtype����Կ���� 
	CString pswCStr="FFFFFFFFFFFF"; // pswCStr��16����CString��Կֵ����Ҫת��Ϊ�ֽ���-> psw
	unsigned char psw[10];		// psw: ��Կֵ
	int asc_len;
	// 2. ����ת����16����CString��Կֵת��Ϊunsigned char*psw
	CDebugger::HexStr2CharArray(pswCStr,psw,&asc_len);
	
	long srcAccount,desAccount, consume;
	CString srcAccountStr,desAccountStr,consumeStr;
	// 3. read_account��ȡԭ���˻��е����
	if(IFD_OK==read_account(page, block, pswtype, psw, &srcAccount)){
		
		// 4. ��ȡ�༭����������ѽ��consumeStr
		((CEdit*)GetDlgItem(IDC_CONSUME_SUM))->GetWindowText(consumeStr);
		if(consumeStr.IsEmpty()){//δ���������״̬�༭�򣬲���m_ValiadBalance������Ϊfalse
			is_failed = true;
			this->m_ValiadBalance = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Amount input is empty!"); //��״̬����ʾ 
			return;
		}
		consume= atol(consumeStr);	 //��CSting ת�� long ����
		if(consume<=0){//������<=0������״̬�༭�򣬲���m_ValiadBalance������Ϊfalse
			is_failed = true;
			this->m_ValiadBalance = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Illegal input!");//��״̬����ʾ  
			return;
		}
		else if(consume>srcAccount){//���ѽ��>��ǰ������״̬�༭�򣬲���m_ValiadBalance������Ϊfalse
			is_failed = true;
			this->m_ValiadBalance = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Insufficient balance!");  //��״̬����ʾ
			return;
		}
		// ���Ѻ����=ԭ�����-���ѽ��
		desAccount = srcAccount-consume;
		if(IFD_OK==(write_account(page, block, pswtype, psw, desAccount))){ 		
			desAccountStr.Format("%ld",desAccount);    // long ת�� Cstirng����
			((CEdit*)GetDlgItem(IDC_BALANCE))->SetWindowText(desAccountStr);//��ʾ���Ѻ�������
			is_failed = false;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Consumed successfully!");//��״̬����ʾ
		
			//////////////////////////////////
			account = desAccount;
			CRecordHelper myrecord;
			CString tmp = consumeStr;
			myrecord.walletRecord(UID,tmp,account,"����");
			OnButtonHistoryLoad();
		}
		else{//д���˻����ʧ��
			is_failed = true;
			((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Failed to consume!");//��״̬����ʾ
		}
	}
	else{//��ȡ�˻����ʧ��
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BALANCE_STATUS))->SetWindowText("Failed to get the account!");//��״̬����ʾ
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
			AfxMessageBox("Cannot link to database.");//�޷��������ݿ�
			return;
		}	
		
		dbHelper.MySQL_ScanOnTable();		
		SetTimer( 2, 1000,NULL);//���ö�ʱ��
		
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
	
	//�û������ϻ�������ʧ��
	if( !dbHelper.MySQL_Query(UID,or) == false ){
		AfxMessageBox("You have already signed in!");
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("User has already signed in.");
		return;
	}
	//�û�����RemainTime���Ҳ����ϻ�����ȡ��ʱ��Ϣ
	else if( !dbHelper.MySQL_Query(UID,rm) == false ){
		currentUserTime = rm; //��ȡʣ��ʱ��
		startTime = currentUserTime;
		
		//��ȡ��ǰϵͳʱ��
		CTime time = CTime::GetCurrentTime();
		//ת����ʽ
		CString startTime_str = time.Format("%Y/%m/%d %H:%M:%S"); 

		struct OnRecord or_tmp(UID , rm, startTime_str ,false); 

		dbHelper.MySQL_Insert(or_tmp);

		CRecordHelper myrecord;
		myrecord.SignInRecord(UID,rm); //��¼����ʷ
		CString pCStr = "User " + UID;
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText(pCStr + " sign in successfully!");
		
		isOnline = true;  //�����ϻ�״̬
		isShowTimeChange = true;   //��ʾʱ������
		setRemainTimeBox(currentUserTime);	//����ʱ��
 
	}
	//�û������ڣ������¶���
	else{
		//��ȡ��ǰϵͳʱ��
		CTime time = CTime::GetCurrentTime();
		//ת����ʽ
		CString startTime_str = time.Format("%Y/%m/%d %H:%M:%S"); 
		
		//��ʼ��ʣ��ʱ��
		currentUserTime = RemainTimeInit;
		startTime = currentUserTime;
		
		//�����������ݿ���¶���
		struct OnRecord or_tmp(UID , RemainTimeInit, startTime_str ,false); 
		struct RemainTime rm_tmp(UID,RemainTimeInit);

		if( !dbHelper.MySQL_Insert( rm_tmp) == true ){
			AfxMessageBox("New user sign up failure.");
		}else if(!dbHelper.MySQL_Insert(or_tmp) == true){
			AfxMessageBox("New user sign in failure.");		
		}
		else{
			CRecordHelper myrecord;
			myrecord.SignInRecord(UID,RemainTimeInit); //��¼����ʷ
			((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText(" Sign up success! Status: online");

			isOnline = true; //�����ϻ�״̬
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
	
	//����OnRecord��
	struct OnRecord or;
	if( !dbHelper.MySQL_Query(UID,or) == false ){  //ʹ�÷��ж�
		// ((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("A");
		//����RemainTimeTable�е�ʣ��ʱ��ֵ
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
		myrecord.ExitRecord(UID,currentUserTime,startTime-currentUserTime); //��¼����ʷ��
		
		emptyRemainTimeBox();  //��նԻ���
		CString pCStr = "User " + UID;
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText(pCStr+" Exit successfully!");
		isOnline = false;  //ȡ���ϻ�״̬
		isShowTimeChange = false;
		UID = ""; //���ȫ�ֱ���UID

	}else {
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Exit failure. User is offline.");
		emptyRemainTimeBox();  //��նԻ���
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

//��ȡʣ������
int CAppDev::getRemainTimeSeconds(CString h,CString m,CString s){
	return atoi(h)*3600+atoi(m)*60+atoi(s)*1;
}

//��������Ƿ�Ϊ������
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

	if( !dbHelper.MySQL_Query(UID,or) == false ){ //ͬʱ�������ϻ������ʱ����
		if(dbHelper.MySQL_UpdateRemainTime(UID,RemainTimeInit,"onlinetable") && dbHelper.MySQL_UpdateRemainTime(UID,RemainTimeInit,"RemainTimeTable")){
			//����ʾʱ������
			isShowTimeChange = false;
			setRemainTimeBox(RemainTimeInit);
			((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("[Online] Set default time successfully.");
			currentUserTime = RemainTimeInit;
		}else{
			AfxMessageBox("[Online] Set default time failure");
		}
	}
	else if( !dbHelper.MySQL_Query(UID,remainTime) == false){	//ֻ������ʱ����
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

	//��ѯ����ʱ����
	int remainTime;
	struct OnRecord or;

	if( !dbHelper.MySQL_Query(UID,or) == false ){
		//��ʾʱ������
		isShowTimeChange = true;  //��ʾʱ������
		currentUserTime = or.RemainSeconds;		
		setRemainTimeBox(currentUserTime);
		((CEdit*)GetDlgItem(IDC_EDIT_STATUS_ONLINE))->SetWindowText("Check time successfully"); 
	}else if(!dbHelper.MySQL_Query(UID,remainTime) == false ){
		isShowTimeChange = true; //��ʾʱ������
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
	//AfxMessageBox("��ʱ��");	
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
	unsigned char uid[100]; //	uid����ȡ��UID��Ϣ
	int uid_len;			//	uid_len����Ϣ�ĳ���
	// 1. ��ȡ����UID
	is_failed = true;
	if (IFD_OK == find_14443(uid,&uid_len)){
		is_failed = false;
	// 2. ����ת��
		// ��unsigned char*���͵�uidת��ΪCString���͵�uidCStr
		   ��ѭ��ƴ�ӵõ�CString 
		uidCStr.Empty();
		for (int i=0;i<uid_len;i++)
		{
			tmp.Format("%02X",uid[i]);	// ����ȡ��UID����(1�ֽ�)תΪ16���ƣ�02x ��ʾ2λ��д��16����
			uidCStr += tmp; 			// CStringƴ��
		}
	// 3. �༭����ʾ����UID��������״̬�༭�򣬻�ȡ���ųɹ�
		((CEdit*)GetDlgItem(IDC_UID))->SetWindowText(uidCStr);
		((CEdit*)GetDlgItem(IDC_INFO_STATUS))->SetWindowText("Get UID successfully");
		return true;
	}else{	// ��ȡʧ��
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
	// ��ȡ����
	
	if(IFD_OK == find_14443(buff,&buff_len)){
		CString a;
		a.Format("%d",buff_len);
		//UID.Empty();
		for(int i=0;i<buff_len;i++){
			tmp.Format("%02x",buff[i]); //����ȡ��uidת����16���Ʊ�����CString���͵ı�����
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
	// ��ȡʱ��
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
	// ��ȡ���
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
//���¶Ի����ʣ��ʱ������
void CAppDev::emptyRemainTimeBox(){
	((CEdit*)GetDlgItem(IDC_EDIT_HOUR))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_MINUTE))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_EDIT_SECOND))->SetWindowText("");
}

//������ʱ���ʣ��ʱ������
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