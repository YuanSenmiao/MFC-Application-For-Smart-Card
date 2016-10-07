// Debugger.cpp : implementation file
//

#include "stdafx.h"
#include "Test0.h"
#include "Debugger.h"
#include "string.h"

#pragma comment(lib,"./libs/ZM124U.lib")
#include "./libs/ZM124U.h"
#import "./libs/msado15.dll" no_namespace rename("EOF","ADOEOF") rename("BOF","ADOBOF")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDebugger dialog


CDebugger::CDebugger(CWnd* pParent /*=NULL*/)
: CDialog(CDebugger::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDebugger)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_RatioKeyType = 0x00;
}


void CDebugger::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDebugger)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDebugger, CDialog)
//{{AFX_MSG_MAP(CDebugger)
ON_BN_CLICKED(IDC_BTN_ACCESS_UID, OnBtnAccessUid)
ON_BN_CLICKED(IDC_BTN_OPEN_DEVICE, OnBtnOpenDevice)
ON_BN_CLICKED(IDC_BTN_CHECK_VERSION, OnBtnCheckVersion)
ON_BN_CLICKED(IDC_BTN_SET_LED, OnBtnSetLed)
ON_BN_CLICKED(IDC_TYPE_A, OnTypeA)
ON_BN_CLICKED(IDC_TYPE_B, OnTypeB)
ON_BN_CLICKED(IDC_BTN_DEF_KEY, OnBtnDefKey)
ON_BN_CLICKED(IDC_BTN_READ_SECTOR, OnBtnReadSector)
ON_BN_CLICKED(IDC_BTN_READ_BLOCK, OnBtnReadBlock)
ON_BN_CLICKED(IDC_BTN_WRITE_BLOCK, OnBtnWriteBlock)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDebugger message handlers
/* 获取卡号UID按钮点击响应函数
   调用接口函数find_14443，
   成功获取后结果显示在UID编辑框，
   否则在状态编辑框显示异常信息*/
void CDebugger::OnBtnAccessUid() 
{
	// TODO: Add your control notification handler code here
	CString uidCStr, tmp;
	unsigned char uid[100];	//	uid：获取的UID信息
	int uid_len;			//	uid_len：信息的长度
	// 1. 获取卡号UID
	is_failed = true;
	if (IFD_OK == find_14443(uid,&uid_len)){
		is_failed = false;
	// 2. 类型转换
		/* 将unsigned char*类型的uid转换为CString类型的uidCStr
		   用循环拼接得到CString */
		uidCStr.Empty();
		for (int i=0;i<uid_len;i++)
		{
			tmp.Format("%02X",uid[i]);  /* 将获取的UID数据(1字节)转为16进制，02x 表示2位大写的16进制 */
			uidCStr += tmp;				/* CString拼接 */
		}
	// 3. 编辑框显示卡号UID，并更新状态编辑框，获取卡号成功
		((CEdit*)GetDlgItem(IDC_UID))->SetWindowText(uidCStr);
		((CEdit*)GetDlgItem(IDC_INFO_STATUS))->SetWindowText("Get UID successfully");
	}else{	/* 获取失败 */
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_UID))->SetWindowText("");
		((CEdit*)GetDlgItem(IDC_INFO_STATUS))->SetWindowText("Get UID failed");
	}
}

////////////////////////////////////////////////////////////////////////////////
/* 开启设备按钮点击响应函数
   调用开场函数IDD_PowerOn，
   将开启结果显示在状态编辑框  */
void CDebugger::OnBtnOpenDevice() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	if(IFD_OK == IDD_PowerOn()){
		((CEdit*)GetDlgItem(IDC_OPEN_STATUS))->SetWindowText("Open device successfully");
		is_failed = false;
	}else{
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_OPEN_STATUS))->SetWindowText("Open device failed");
	}
}

////////////////////////////////////////////////////////////////////////////////
/*	查看版本信息按钮点击响应函数
	调用接口函数Reader_Version
	成功获取后将结果显示在编辑框，
	否则在状态编辑框显示失败信息 */
void CDebugger::OnBtnCheckVersion() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	CString version_info, tmp;// version_info 用于给编辑框赋值,tmp用来装类型转换的每个ascii字符

	/*	读取读卡器版本信息函数Reader_Version的参数
		resp：		返回的ATR信息
		resp_len：	返回的ATR信息的长度 */
	unsigned char resp[1000];
	int resp_len; 
	// 1. 获取版本信息
	if(IFD_OK == Reader_Version(resp,&resp_len)){
	// 2. 类型转换
		/* 将unsigned char*类型的 resp 转换为CString类型的 version_info
		   用循环拼接得到CString */
		version_info.Empty();			  /* 清空字符串以拼接 */
		is_failed = false;
		for (int i=0;i<resp_len;i++)
		{
			tmp.Format("%c",resp[i]);     /* 获取得到的版本号是unsigned char数组，使用SetWindowText时
										     参数是CString类型，需将unsigned char数组转ascii字符串 */
			version_info += tmp;		  /* CString拼接 */
		}
	// 3. 显示完整版本信息，并更新状态编辑框，查看版本信息成功
		((CEdit*)GetDlgItem(IDC_VERSION))->SetWindowText(version_info);
		((CEdit*)GetDlgItem(IDC_INFO_STATUS))->SetWindowText("Check version successfully");
	}else{/* 获取失败 */
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_VERSION))->SetWindowText("");
		((CEdit*)GetDlgItem(IDC_INFO_STATUS))->SetWindowText("Check version failed");
	}	
}

////////////////////////////////////////////////////////////////////////////////
/* 设置LED灯
	调用接口函数LED */
void CDebugger::OnBtnSetLed() 
{
	// TODO: Add your control notification handler code here
	// 1. 获取输入设置值
	CString set_info,set_points; // 输入的信息和小数点设置值
	((CEdit*)GetDlgItem(IDC_LED_INFO))->GetWindowText(set_info);	/* 获取信息设置值		*/
	((CEdit*)GetDlgItem(IDC_DEC_DIG))->GetWindowText(set_points);	/* 获取小数位数设置值	*/
	
	if(set_info.IsEmpty()){ /* 信息输入为空 */
		MessageBox("Please enter the information data!");/* 弹窗提醒用户输入信息 */
		return;
	}
	if(set_points.IsEmpty()){ /* 小数点位数输入为空 */
		MessageBox("Please enter the decimal points!");/* 弹窗提醒用户输入小数位数 */
		return;
	}

	/* LED函数参数 */
	char data[100];	// data：LED灯显示的信息
	int data_len;	// data_len：信息的长度
	unsigned char point; // point：小数点位数(0~6)
	// 2. 设置参数值
	/* 从控件获取的数据到使用参数都需要类型转换 */
	strncpy(data,(LPCTSTR)set_info,sizeof(data));  /* 将信息输入值复制给data，CString类型转为char* */
    point =atoi(set_points)+1;  /* 将CString类型转换为unsigned char类型 */
    data_len=strlen(data);		/* 赋值给data_len */

	is_failed = true;
	// 3.调用LED接口函数设置LED灯 
	if(IFD_OK == LED(data,data_len,point)){
		is_failed = false;
		((CEdit*)GetDlgItem(IDC_LED_STATUS))->SetWindowText("Set LED successfully");
	}else{
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_LED_STATUS))->SetWindowText("Set LED failed");
	}
}

////////////////////////////////////////////////////////////////////////////////
/*	密钥类型是由成员变量m_RatioKeyType记录下的类型，
	根据m_RatioKeyType判断选择的密钥是“0x0A”还“0x0B”*/
void CDebugger::OnTypeA() 
{
	// TODO: Add your control notification handler code here
	m_RatioKeyType=0x0A;
}

////////////////////////////////////////////////////////////////////////////////
void CDebugger::OnTypeB() 
{
	// TODO: Add your control notification handler code here
	m_RatioKeyType=0x0B;
}

////////////////////////////////////////////////////////////////////////////////
/* 默认密钥点击响应函数，设置密钥为默认值FFFFFFFFFFFF
   并显示在编辑框 */
void CDebugger::OnBtnDefKey() 
{
	// TODO: Add your control notification handler code here
	// 2. 获取密钥类型
	if(m_RatioKeyType!=0x0A && m_RatioKeyType!=0x0B)
	{
		MessageBox("Undefined key type!");
		return;
	}
	((CEdit*)GetDlgItem(IDC_KEY))->SetWindowText("FFFFFFFFFFFF");
}

////////////////////////////////////////////////////////////////////////////////
/* 将16进制 CString 类型转化为字节流unsigned char* 类型  */
void CDebugger::HexStr2CharArray(CString hexStr,unsigned char *asc,int *asc_len)
{
	*asc_len=0;
	int len = hexStr.GetLength();
	char* temp=(char *)malloc(len+1);//给临时数组分配空间，注意是hexStrLen+1
	char tmp[3]={0};
	char *Hex;
	unsigned char*p;//指针变量，用于储存字符并给数组赋值
	
	strncpy(temp,(LPCTSTR)hexStr,len+1);
	Hex = temp;
	p= asc;
	
	while(*Hex!='\0')
	{
		tmp[0]=*Hex;
		Hex++;
		tmp[1]=*Hex;
		Hex++;
		tmp[2]='\0';
		*p =(unsigned char) strtol(tmp,NULL,16);//类型转换，并赋值给*p
		p++;
		(*asc_len)++;
	}
	*p='\0';	//char数组终止符
	free(temp);	//销毁临时数组的指针
}

////////////////////////////////////////////////////////////////////////////////
/* 读扇区 */
void CDebugger::OnBtnReadSector() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	// 1. 读扇区之前先清除所有块编辑框的内容，否则遗留上一次信息
    ((CEdit*)GetDlgItem(IDC_BLOCK0))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK1))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK2))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_A))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_B))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_C))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText("");
	// 2. 获取密钥类型
	if(m_RatioKeyType!=0x0A && m_RatioKeyType!=0x0B) /* 检查密钥类型是否已经指定 */
	{
		MessageBox("Undefined key type!");	/* 弹窗提醒用户选择密钥类型 */
		return;
	}
	
	// 3. 获取密钥
	CString key;	// CString类型密钥变量key
	((CEdit*)GetDlgItem(IDC_KEY))->GetWindowText(key);  /* 获取密钥编辑框的内容，赋值到key */	
	if(key.IsEmpty()){ /* 密钥输入为空 */
		MessageBox("Please enter the key!");/* 弹窗提醒用户输入密钥 */
		return;
	}
	// 4. 获取选择的扇区
	int page = ((CComboBox*)GetDlgItem(IDC_SECTOR))->GetCurSel(); //从组合框获取选择的扇区号
    if(CB_ERR == page){ /* 用户没有选择扇区号 */
		MessageBox("Please select a sector!");	/* 弹框提醒选择扇区号 */
		return;
	}else{
		page += 1; // 扇区号=组合框下标+1
	}

    // 5. 对每一个块都调用函数read_block（注意函数参数的类型）
	CString tmp,blockData,block3A,block3B,block3C;
	unsigned char psw[10];
	unsigned char des_data[100];
	int des_len;
	int asc_len;
	int block;
	// 5.1 将密钥从16进制CString转化为unsigned char*
	HexStr2CharArray(key,psw,&asc_len);
	bool successReadSector = true;
	// 5.2 读取块0数据
	block=0;
	if((IFD_OK == read_block(page,block,m_RatioKeyType,psw,des_data,&des_len))){
		is_failed = false;
		// 类型转换，将返回的unsigned char类型的块数据des_data转为Cstring类型
		blockData.Empty();
		for (int i=0;i<des_len;i++)
		{
			tmp.Format("%02x",des_data[i]); /* 将unsigned char类型转为Cstring类型 */ 
			blockData += tmp; /* CString拼接 */
		}
		((CEdit*)GetDlgItem(IDC_BLOCK0))->SetWindowText(blockData);
	}else{  /* 读取失败，在相应编辑框显示状态信息 */
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BLOCK0))->SetWindowText("Read block 0 failed!");
		successReadSector = false;
	}
	/* 读取块1数据 */
	block=1;
	if((IFD_OK == read_block(page,block,m_RatioKeyType,psw,des_data,&des_len))){
		is_failed = false;
		blockData.Empty();
		for (int i=0;i<des_len;i++)
		{
			tmp.Format("%02x",des_data[i]);
			blockData += tmp;
		}
		((CEdit*)GetDlgItem(IDC_BLOCK1))->SetWindowText(blockData);
	}
	else{
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BLOCK1))->SetWindowText("Read block 1 failed!");
		successReadSector = false;
	}
	
	/* 读取块2数据 */
	block=2;
	if((IFD_OK == read_block(page,block,m_RatioKeyType,psw,des_data,&des_len))){
		is_failed = false;
		blockData.Empty();
		for (int i=0;i<des_len;i++)
		{
			tmp.Format("%02x",des_data[i]); 
			blockData += tmp;
		}
		((CEdit*)GetDlgItem(IDC_BLOCK2))->SetWindowText(blockData);
	}
	else{
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BLOCK2))->SetWindowText("Read block 2 failed!");
		successReadSector = false;
	}
	
	/* 读取块3数据 */
	block=3;
	if((IFD_OK == read_block(page,block,m_RatioKeyType,psw,des_data,&des_len))){
		is_failed = false;
		block3A.Empty();
		for(int i=0;i<6;i++)
		{
			tmp.Format("%02x",des_data[i]);     /* 将unsigned char类型转为Cstring类型 */
			block3A += tmp;
		}
		block3B.Empty();
		for(int j=6;j<10;j++)
		{
			tmp.Format("%02x",des_data[j]);     /* 将unsigned char类型转为Cstring类型 */
			block3B += tmp;
		}
		block3C.Empty();
		for(int k=10;k<16;k++)
		{
			tmp.Format("%02x",des_data[k]);     /* 将unsigned char类型转为Cstring类型 */
			block3C += tmp;
		}
		/*  将数据填到A,B,C三个编辑框中  */
		((CEdit*)GetDlgItem(IDC_BLOCK3_A))->SetWindowText(block3A);
		((CEdit*)GetDlgItem(IDC_BLOCK3_B))->SetWindowText(block3B);
		((CEdit*)GetDlgItem(IDC_BLOCK3_C))->SetWindowText(block3C);
	}else{
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BLOCK2))->SetWindowText("Read block 3 failed!");
		successReadSector = false;
	}
	if(successReadSector){
		// 5.3 显示读扇区成功，read sector x successfully!
		CString msg,sectorNum;
		sectorNum.Format("%d",page);
		msg = "Read sector "+sectorNum;
		msg = msg + " successfully!";
		is_failed = false;
		((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText(msg);
	}
		
}

////////////////////////////////////////////////////////////////////////////////
/*   读块数据，根据选择的块区号对块区进行读取数据
	 调用read_block函数  */
void CDebugger::OnBtnReadBlock() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	// 1. 读块之前先清除所有块编辑框的内容
    ((CEdit*)GetDlgItem(IDC_BLOCK0))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK1))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK2))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_A))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_B))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_C))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText("");
	// 2. 获取密钥类型
	if(m_RatioKeyType!=0x0A && m_RatioKeyType!=0x0B)
	{
		MessageBox("Undefined key type!");
		return;
	}
	
	// 3. 获取密钥
	CString key;	// CString类型密钥变量key
	((CEdit*)GetDlgItem(IDC_KEY))->GetWindowText(key);  /* 获取密钥编辑框的内容，赋值到key */	
	if(key.IsEmpty()){ /* 密钥输入为空 */
		MessageBox("Please enter the key!");/* 弹窗提醒用户输入密钥 */
		return;
	}
	// 4. 获取扇区号和块区号
    int page=((CComboBox*)GetDlgItem(IDC_SECTOR))->GetCurSel(); //获取扇区号
	int block=((CComboBox*)GetDlgItem(IDC_BLOCK))->GetCurSel();	//获取块区号
	
	if(CB_ERR == page){
		MessageBox("Please select a sector!");
		return;
	}
	else{
		page += 1;// 扇区号=组合框下标+1
	}
	if(CB_ERR == block){
		MessageBox("Please select a block!");
		return;
	}
	
	// 需要的参数都已经获取，使用函数读取块数据。注意的是，读取的数据是以数组形式(unsigned char data[1024])，
	// 填入编辑框时，要转换为CString类型；
	// 5. 对选择的块区调用函数read_block接口函数（注意函数参数的类型）
	CString tmp,blockData,block3A,block3B,block3C;
	unsigned char psw[10];		//psw：密钥值
	unsigned char des_data[10];	//des_data：读取的信息
	int des_len;//des_len： 信息的长度
	int asc_len;    
	// 5.1 类型转换
    HexStr2CharArray(key,psw,&asc_len);  /* 密钥16进制CString转化为unsigned char类型 */
	// 5.2 读块区获取块区数据，填入编辑框
	if((IFD_OK == read_block(page,block,m_RatioKeyType,psw,des_data,&des_len))){
		blockData.Empty();		
		block3A.Empty();
		block3B.Empty();
		block3C.Empty();
		if(block==0){	// 块0		
			for (int i=0;i<des_len;i++)
			{
				tmp.Format("%02x",des_data[i]); //将 unsigned char类型转为Cstring 类型
				blockData += tmp;
			}
			((CEdit*)GetDlgItem(IDC_BLOCK0))->SetWindowText(blockData);
		}
		else if(block==1){	// 块1
			for (int i=0;i<des_len;i++)
			{
				tmp.Format("%02x",des_data[i]); 
				blockData += tmp;
			}
			((CEdit*)GetDlgItem(IDC_BLOCK1))->SetWindowText(blockData);
		}
		else if(block==2){	// 块2
			for (int i=0;i<des_len;i++)
			{
				tmp.Format("%02x",des_data[i]);
				blockData += tmp;
			}
			((CEdit*)GetDlgItem(IDC_BLOCK2))->SetWindowText(blockData);
		}
		// 若选择的是块3，则填入编辑框时要将数组拆分成三个部分
		else if(block==3){	// 块3		
			for(int i=0;i<6;i++)	// 块3A，第一部分是6个字节
			{
				tmp.Format("%02x",des_data[i]); 
				block3A += tmp;
			}
			for(int j=6;j<10;j++)	// 块3B，第二部分是4个字节
			{
				tmp.Format("%02x",des_data[j]); 
				block3B += tmp;
			}
			for(int k=10;k<16;k++)	// 块3C，第三部分是6个字节；
			{
				tmp.Format("%02x",des_data[k]);
				block3C += tmp;
			}
			((CEdit*)GetDlgItem(IDC_BLOCK3_A))->SetWindowText(block3A);
			((CEdit*)GetDlgItem(IDC_BLOCK3_B))->SetWindowText(block3B);
			((CEdit*)GetDlgItem(IDC_BLOCK3_C))->SetWindowText(block3C);
		}
		// 5.3 显示读块成功
		CString msg,blockNum;
		blockNum.Format("%d",block);
		msg = "Read block "+blockNum;
		msg = msg + " successfully!";
		is_failed = false;
		((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText(msg);
	}
	else{
		CString msg,blockNum;
		blockNum.Format("%d",block);
		msg = "Read block "+blockNum;
		msg = msg + " failed!";
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText(msg);
	}
}

////////////////////////////////////////////////////////////////////////////////
/*   写块数据，对选择的块区号对块区进行写入数据 
	 调用write_block 函数 */
void CDebugger::OnBtnWriteBlock() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText("");
	// 1. 获取密钥类型
	if(m_RatioKeyType!=0x0A && m_RatioKeyType!=0x0B)
	{
		MessageBox("Undefined key type!");
		return;
	}
	// 2. 获取密钥
	CString key;
	((CEdit*)GetDlgItem(IDC_KEY))->GetWindowText(key);   /* 获取密钥编辑框的内容 */
	if(key.IsEmpty()){
		MessageBox("Please enter the key!");
		return;
	}
	// 3. 获取扇区号
	int page=((CComboBox*)GetDlgItem(IDC_SECTOR))->GetCurSel(); //获取扇区号
	if(CB_ERR == page){
		MessageBox("Please select a sector!");
		return;
	}
	else	
		page += 1;// 扇区号=组合框下标+1
	
	// 4. 获取块区号
	int block=((CComboBox*)GetDlgItem(IDC_BLOCK))->GetCurSel();//获取块区号
	if(CB_ERR == block){
		MessageBox("Please select a block!");
		return;
	}

	// 5. 写入块区数据

	// 5.1 密钥格式转换
	unsigned char psw[100];//psw：密钥值
	int asc_len;//unsigned char数组大小
    HexStr2CharArray(key,psw,&asc_len);// 将密钥从16进制CString转化为unsigned char*

	// 5.2 获取写入块区数据
	CString blockData,block3A,block3B,block3C;	
	switch(block)
	{
	case 0:
		((CEdit*)GetDlgItem(IDC_BLOCK0))->GetWindowText(blockData);  //获取块0的数据
		break;
	case 1:
		((CEdit*)GetDlgItem(IDC_BLOCK1))->GetWindowText(blockData);  //获取块1的数据
		break;
	case 2:
		((CEdit*)GetDlgItem(IDC_BLOCK2))->GetWindowText(blockData);  //获取块2的数据
		break;
	case 3:
		MessageBox("Cannot write block 3!");
		return;
	default:
		break;
	}
	if(blockData.IsEmpty()){//如果用户没有输入块数据，弹窗提醒
		MessageBox("Please enter the block data!");
		return;
	}

	if(blockData.GetLength()%2==1)/* 如果数据的长度为奇数，就在前面补个0 */
		blockData='0'+blockData; 

	// 5.3 对获取的数据进行处理
	unsigned char src_data[100];//src_data：写入的信息数据
	int src_len;//src_len： 信息的长度
	HexStr2CharArray(blockData,src_data,&asc_len);   // 将16进制CString的blockData转换为字节流unsigned char*
	src_len=asc_len;
	// 5.4 将读取的数据写入相应的块中	
	//	对选择的块区调用函数write_block接口函数
	//	若写入失败，则在“状态”编辑框中提示“Write block x failed”；若成功则显示“Write block successfully!”
	if(IFD_OK ==write_block(block,page,m_RatioKeyType,psw,src_data,src_len)){
		is_failed = false;
		((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText("Write block successfully!");
	}
	else{
		is_failed = true;
		CString msg,blockNum;
		blockNum.Format("%d",block);
		msg = "Write block "+blockNum;
		msg = msg + " failed!";
		((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText(msg);
	}		
}

HBRUSH CDebugger::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if(pWnd->GetDlgCtrlID() == IDC_OPEN_STATUS
		||pWnd->GetDlgCtrlID() == IDC_INFO_STATUS
		||pWnd->GetDlgCtrlID() == IDC_LED_STATUS
		||pWnd->GetDlgCtrlID() == IDC_RW_STATUS)	
	{
		if(!is_failed)
			pDC->SetTextColor(RGB(0,0,255));
		else
			pDC->SetTextColor(RGB(255,0,0));
	}
	if(pWnd->GetDlgCtrlID() == IDC_BLOCK0
		||pWnd->GetDlgCtrlID() == IDC_BLOCK1
		||pWnd->GetDlgCtrlID() == IDC_BLOCK2
		||pWnd->GetDlgCtrlID() == IDC_BLOCK3_A
		||pWnd->GetDlgCtrlID() == IDC_BLOCK3_B
		||pWnd->GetDlgCtrlID() == IDC_BLOCK3_C)	
	{
		if(!is_failed)
			pDC->SetTextColor(RGB(0,0,0));
		else
			pDC->SetTextColor(RGB(255,0,0));
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
