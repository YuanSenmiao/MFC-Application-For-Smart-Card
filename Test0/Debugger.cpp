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
/* ��ȡ����UID��ť�����Ӧ����
   ���ýӿں���find_14443��
   �ɹ���ȡ������ʾ��UID�༭��
   ������״̬�༭����ʾ�쳣��Ϣ*/
void CDebugger::OnBtnAccessUid() 
{
	// TODO: Add your control notification handler code here
	CString uidCStr, tmp;
	unsigned char uid[100];	//	uid����ȡ��UID��Ϣ
	int uid_len;			//	uid_len����Ϣ�ĳ���
	// 1. ��ȡ����UID
	is_failed = true;
	if (IFD_OK == find_14443(uid,&uid_len)){
		is_failed = false;
	// 2. ����ת��
		/* ��unsigned char*���͵�uidת��ΪCString���͵�uidCStr
		   ��ѭ��ƴ�ӵõ�CString */
		uidCStr.Empty();
		for (int i=0;i<uid_len;i++)
		{
			tmp.Format("%02X",uid[i]);  /* ����ȡ��UID����(1�ֽ�)תΪ16���ƣ�02x ��ʾ2λ��д��16���� */
			uidCStr += tmp;				/* CStringƴ�� */
		}
	// 3. �༭����ʾ����UID��������״̬�༭�򣬻�ȡ���ųɹ�
		((CEdit*)GetDlgItem(IDC_UID))->SetWindowText(uidCStr);
		((CEdit*)GetDlgItem(IDC_INFO_STATUS))->SetWindowText("Get UID successfully");
	}else{	/* ��ȡʧ�� */
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_UID))->SetWindowText("");
		((CEdit*)GetDlgItem(IDC_INFO_STATUS))->SetWindowText("Get UID failed");
	}
}

////////////////////////////////////////////////////////////////////////////////
/* �����豸��ť�����Ӧ����
   ���ÿ�������IDD_PowerOn��
   �����������ʾ��״̬�༭��  */
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
/*	�鿴�汾��Ϣ��ť�����Ӧ����
	���ýӿں���Reader_Version
	�ɹ���ȡ�󽫽����ʾ�ڱ༭��
	������״̬�༭����ʾʧ����Ϣ */
void CDebugger::OnBtnCheckVersion() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	CString version_info, tmp;// version_info ���ڸ��༭��ֵ,tmp����װ����ת����ÿ��ascii�ַ�

	/*	��ȡ�������汾��Ϣ����Reader_Version�Ĳ���
		resp��		���ص�ATR��Ϣ
		resp_len��	���ص�ATR��Ϣ�ĳ��� */
	unsigned char resp[1000];
	int resp_len; 
	// 1. ��ȡ�汾��Ϣ
	if(IFD_OK == Reader_Version(resp,&resp_len)){
	// 2. ����ת��
		/* ��unsigned char*���͵� resp ת��ΪCString���͵� version_info
		   ��ѭ��ƴ�ӵõ�CString */
		version_info.Empty();			  /* ����ַ�����ƴ�� */
		is_failed = false;
		for (int i=0;i<resp_len;i++)
		{
			tmp.Format("%c",resp[i]);     /* ��ȡ�õ��İ汾����unsigned char���飬ʹ��SetWindowTextʱ
										     ������CString���ͣ��轫unsigned char����תascii�ַ��� */
			version_info += tmp;		  /* CStringƴ�� */
		}
	// 3. ��ʾ�����汾��Ϣ��������״̬�༭�򣬲鿴�汾��Ϣ�ɹ�
		((CEdit*)GetDlgItem(IDC_VERSION))->SetWindowText(version_info);
		((CEdit*)GetDlgItem(IDC_INFO_STATUS))->SetWindowText("Check version successfully");
	}else{/* ��ȡʧ�� */
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_VERSION))->SetWindowText("");
		((CEdit*)GetDlgItem(IDC_INFO_STATUS))->SetWindowText("Check version failed");
	}	
}

////////////////////////////////////////////////////////////////////////////////
/* ����LED��
	���ýӿں���LED */
void CDebugger::OnBtnSetLed() 
{
	// TODO: Add your control notification handler code here
	// 1. ��ȡ��������ֵ
	CString set_info,set_points; // �������Ϣ��С��������ֵ
	((CEdit*)GetDlgItem(IDC_LED_INFO))->GetWindowText(set_info);	/* ��ȡ��Ϣ����ֵ		*/
	((CEdit*)GetDlgItem(IDC_DEC_DIG))->GetWindowText(set_points);	/* ��ȡС��λ������ֵ	*/
	
	if(set_info.IsEmpty()){ /* ��Ϣ����Ϊ�� */
		MessageBox("Please enter the information data!");/* ���������û�������Ϣ */
		return;
	}
	if(set_points.IsEmpty()){ /* С����λ������Ϊ�� */
		MessageBox("Please enter the decimal points!");/* ���������û�����С��λ�� */
		return;
	}

	/* LED�������� */
	char data[100];	// data��LED����ʾ����Ϣ
	int data_len;	// data_len����Ϣ�ĳ���
	unsigned char point; // point��С����λ��(0~6)
	// 2. ���ò���ֵ
	/* �ӿؼ���ȡ�����ݵ�ʹ�ò�������Ҫ����ת�� */
	strncpy(data,(LPCTSTR)set_info,sizeof(data));  /* ����Ϣ����ֵ���Ƹ�data��CString����תΪchar* */
    point =atoi(set_points)+1;  /* ��CString����ת��Ϊunsigned char���� */
    data_len=strlen(data);		/* ��ֵ��data_len */

	is_failed = true;
	// 3.����LED�ӿں�������LED�� 
	if(IFD_OK == LED(data,data_len,point)){
		is_failed = false;
		((CEdit*)GetDlgItem(IDC_LED_STATUS))->SetWindowText("Set LED successfully");
	}else{
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_LED_STATUS))->SetWindowText("Set LED failed");
	}
}

////////////////////////////////////////////////////////////////////////////////
/*	��Կ�������ɳ�Ա����m_RatioKeyType��¼�µ����ͣ�
	����m_RatioKeyType�ж�ѡ�����Կ�ǡ�0x0A������0x0B��*/
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
/* Ĭ����Կ�����Ӧ������������ԿΪĬ��ֵFFFFFFFFFFFF
   ����ʾ�ڱ༭�� */
void CDebugger::OnBtnDefKey() 
{
	// TODO: Add your control notification handler code here
	// 2. ��ȡ��Կ����
	if(m_RatioKeyType!=0x0A && m_RatioKeyType!=0x0B)
	{
		MessageBox("Undefined key type!");
		return;
	}
	((CEdit*)GetDlgItem(IDC_KEY))->SetWindowText("FFFFFFFFFFFF");
}

////////////////////////////////////////////////////////////////////////////////
/* ��16���� CString ����ת��Ϊ�ֽ���unsigned char* ����  */
void CDebugger::HexStr2CharArray(CString hexStr,unsigned char *asc,int *asc_len)
{
	*asc_len=0;
	int len = hexStr.GetLength();
	char* temp=(char *)malloc(len+1);//����ʱ�������ռ䣬ע����hexStrLen+1
	char tmp[3]={0};
	char *Hex;
	unsigned char*p;//ָ����������ڴ����ַ��������鸳ֵ
	
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
		*p =(unsigned char) strtol(tmp,NULL,16);//����ת��������ֵ��*p
		p++;
		(*asc_len)++;
	}
	*p='\0';	//char������ֹ��
	free(temp);	//������ʱ�����ָ��
}

////////////////////////////////////////////////////////////////////////////////
/* ������ */
void CDebugger::OnBtnReadSector() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	// 1. ������֮ǰ��������п�༭������ݣ�����������һ����Ϣ
    ((CEdit*)GetDlgItem(IDC_BLOCK0))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK1))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK2))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_A))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_B))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_C))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText("");
	// 2. ��ȡ��Կ����
	if(m_RatioKeyType!=0x0A && m_RatioKeyType!=0x0B) /* �����Կ�����Ƿ��Ѿ�ָ�� */
	{
		MessageBox("Undefined key type!");	/* ���������û�ѡ����Կ���� */
		return;
	}
	
	// 3. ��ȡ��Կ
	CString key;	// CString������Կ����key
	((CEdit*)GetDlgItem(IDC_KEY))->GetWindowText(key);  /* ��ȡ��Կ�༭������ݣ���ֵ��key */	
	if(key.IsEmpty()){ /* ��Կ����Ϊ�� */
		MessageBox("Please enter the key!");/* ���������û�������Կ */
		return;
	}
	// 4. ��ȡѡ�������
	int page = ((CComboBox*)GetDlgItem(IDC_SECTOR))->GetCurSel(); //����Ͽ��ȡѡ���������
    if(CB_ERR == page){ /* �û�û��ѡ�������� */
		MessageBox("Please select a sector!");	/* ��������ѡ�������� */
		return;
	}else{
		page += 1; // ������=��Ͽ��±�+1
	}

    // 5. ��ÿһ���鶼���ú���read_block��ע�⺯�����������ͣ�
	CString tmp,blockData,block3A,block3B,block3C;
	unsigned char psw[10];
	unsigned char des_data[100];
	int des_len;
	int asc_len;
	int block;
	// 5.1 ����Կ��16����CStringת��Ϊunsigned char*
	HexStr2CharArray(key,psw,&asc_len);
	bool successReadSector = true;
	// 5.2 ��ȡ��0����
	block=0;
	if((IFD_OK == read_block(page,block,m_RatioKeyType,psw,des_data,&des_len))){
		is_failed = false;
		// ����ת���������ص�unsigned char���͵Ŀ�����des_dataתΪCstring����
		blockData.Empty();
		for (int i=0;i<des_len;i++)
		{
			tmp.Format("%02x",des_data[i]); /* ��unsigned char����תΪCstring���� */ 
			blockData += tmp; /* CStringƴ�� */
		}
		((CEdit*)GetDlgItem(IDC_BLOCK0))->SetWindowText(blockData);
	}else{  /* ��ȡʧ�ܣ�����Ӧ�༭����ʾ״̬��Ϣ */
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BLOCK0))->SetWindowText("Read block 0 failed!");
		successReadSector = false;
	}
	/* ��ȡ��1���� */
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
	
	/* ��ȡ��2���� */
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
	
	/* ��ȡ��3���� */
	block=3;
	if((IFD_OK == read_block(page,block,m_RatioKeyType,psw,des_data,&des_len))){
		is_failed = false;
		block3A.Empty();
		for(int i=0;i<6;i++)
		{
			tmp.Format("%02x",des_data[i]);     /* ��unsigned char����תΪCstring���� */
			block3A += tmp;
		}
		block3B.Empty();
		for(int j=6;j<10;j++)
		{
			tmp.Format("%02x",des_data[j]);     /* ��unsigned char����תΪCstring���� */
			block3B += tmp;
		}
		block3C.Empty();
		for(int k=10;k<16;k++)
		{
			tmp.Format("%02x",des_data[k]);     /* ��unsigned char����תΪCstring���� */
			block3C += tmp;
		}
		/*  �������A,B,C�����༭����  */
		((CEdit*)GetDlgItem(IDC_BLOCK3_A))->SetWindowText(block3A);
		((CEdit*)GetDlgItem(IDC_BLOCK3_B))->SetWindowText(block3B);
		((CEdit*)GetDlgItem(IDC_BLOCK3_C))->SetWindowText(block3C);
	}else{
		is_failed = true;
		((CEdit*)GetDlgItem(IDC_BLOCK2))->SetWindowText("Read block 3 failed!");
		successReadSector = false;
	}
	if(successReadSector){
		// 5.3 ��ʾ�������ɹ���read sector x successfully!
		CString msg,sectorNum;
		sectorNum.Format("%d",page);
		msg = "Read sector "+sectorNum;
		msg = msg + " successfully!";
		is_failed = false;
		((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText(msg);
	}
		
}

////////////////////////////////////////////////////////////////////////////////
/*   �������ݣ�����ѡ��Ŀ����ŶԿ������ж�ȡ����
	 ����read_block����  */
void CDebugger::OnBtnReadBlock() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	// 1. ����֮ǰ��������п�༭�������
    ((CEdit*)GetDlgItem(IDC_BLOCK0))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK1))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK2))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_A))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_B))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_BLOCK3_C))->SetWindowText("");
	((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText("");
	// 2. ��ȡ��Կ����
	if(m_RatioKeyType!=0x0A && m_RatioKeyType!=0x0B)
	{
		MessageBox("Undefined key type!");
		return;
	}
	
	// 3. ��ȡ��Կ
	CString key;	// CString������Կ����key
	((CEdit*)GetDlgItem(IDC_KEY))->GetWindowText(key);  /* ��ȡ��Կ�༭������ݣ���ֵ��key */	
	if(key.IsEmpty()){ /* ��Կ����Ϊ�� */
		MessageBox("Please enter the key!");/* ���������û�������Կ */
		return;
	}
	// 4. ��ȡ�����źͿ�����
    int page=((CComboBox*)GetDlgItem(IDC_SECTOR))->GetCurSel(); //��ȡ������
	int block=((CComboBox*)GetDlgItem(IDC_BLOCK))->GetCurSel();	//��ȡ������
	
	if(CB_ERR == page){
		MessageBox("Please select a sector!");
		return;
	}
	else{
		page += 1;// ������=��Ͽ��±�+1
	}
	if(CB_ERR == block){
		MessageBox("Please select a block!");
		return;
	}
	
	// ��Ҫ�Ĳ������Ѿ���ȡ��ʹ�ú�����ȡ�����ݡ�ע����ǣ���ȡ����������������ʽ(unsigned char data[1024])��
	// ����༭��ʱ��Ҫת��ΪCString���ͣ�
	// 5. ��ѡ��Ŀ������ú���read_block�ӿں�����ע�⺯�����������ͣ�
	CString tmp,blockData,block3A,block3B,block3C;
	unsigned char psw[10];		//psw����Կֵ
	unsigned char des_data[10];	//des_data����ȡ����Ϣ
	int des_len;//des_len�� ��Ϣ�ĳ���
	int asc_len;    
	// 5.1 ����ת��
    HexStr2CharArray(key,psw,&asc_len);  /* ��Կ16����CStringת��Ϊunsigned char���� */
	// 5.2 ��������ȡ�������ݣ�����༭��
	if((IFD_OK == read_block(page,block,m_RatioKeyType,psw,des_data,&des_len))){
		blockData.Empty();		
		block3A.Empty();
		block3B.Empty();
		block3C.Empty();
		if(block==0){	// ��0		
			for (int i=0;i<des_len;i++)
			{
				tmp.Format("%02x",des_data[i]); //�� unsigned char����תΪCstring ����
				blockData += tmp;
			}
			((CEdit*)GetDlgItem(IDC_BLOCK0))->SetWindowText(blockData);
		}
		else if(block==1){	// ��1
			for (int i=0;i<des_len;i++)
			{
				tmp.Format("%02x",des_data[i]); 
				blockData += tmp;
			}
			((CEdit*)GetDlgItem(IDC_BLOCK1))->SetWindowText(blockData);
		}
		else if(block==2){	// ��2
			for (int i=0;i<des_len;i++)
			{
				tmp.Format("%02x",des_data[i]);
				blockData += tmp;
			}
			((CEdit*)GetDlgItem(IDC_BLOCK2))->SetWindowText(blockData);
		}
		// ��ѡ����ǿ�3��������༭��ʱҪ�������ֳ���������
		else if(block==3){	// ��3		
			for(int i=0;i<6;i++)	// ��3A����һ������6���ֽ�
			{
				tmp.Format("%02x",des_data[i]); 
				block3A += tmp;
			}
			for(int j=6;j<10;j++)	// ��3B���ڶ�������4���ֽ�
			{
				tmp.Format("%02x",des_data[j]); 
				block3B += tmp;
			}
			for(int k=10;k<16;k++)	// ��3C������������6���ֽڣ�
			{
				tmp.Format("%02x",des_data[k]);
				block3C += tmp;
			}
			((CEdit*)GetDlgItem(IDC_BLOCK3_A))->SetWindowText(block3A);
			((CEdit*)GetDlgItem(IDC_BLOCK3_B))->SetWindowText(block3B);
			((CEdit*)GetDlgItem(IDC_BLOCK3_C))->SetWindowText(block3C);
		}
		// 5.3 ��ʾ����ɹ�
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
/*   д�����ݣ���ѡ��Ŀ����ŶԿ�������д������ 
	 ����write_block ���� */
void CDebugger::OnBtnWriteBlock() 
{
	// TODO: Add your control notification handler code here
	is_failed = true;
	((CEdit*)GetDlgItem(IDC_RW_STATUS))->SetWindowText("");
	// 1. ��ȡ��Կ����
	if(m_RatioKeyType!=0x0A && m_RatioKeyType!=0x0B)
	{
		MessageBox("Undefined key type!");
		return;
	}
	// 2. ��ȡ��Կ
	CString key;
	((CEdit*)GetDlgItem(IDC_KEY))->GetWindowText(key);   /* ��ȡ��Կ�༭������� */
	if(key.IsEmpty()){
		MessageBox("Please enter the key!");
		return;
	}
	// 3. ��ȡ������
	int page=((CComboBox*)GetDlgItem(IDC_SECTOR))->GetCurSel(); //��ȡ������
	if(CB_ERR == page){
		MessageBox("Please select a sector!");
		return;
	}
	else	
		page += 1;// ������=��Ͽ��±�+1
	
	// 4. ��ȡ������
	int block=((CComboBox*)GetDlgItem(IDC_BLOCK))->GetCurSel();//��ȡ������
	if(CB_ERR == block){
		MessageBox("Please select a block!");
		return;
	}

	// 5. д���������

	// 5.1 ��Կ��ʽת��
	unsigned char psw[100];//psw����Կֵ
	int asc_len;//unsigned char�����С
    HexStr2CharArray(key,psw,&asc_len);// ����Կ��16����CStringת��Ϊunsigned char*

	// 5.2 ��ȡд���������
	CString blockData,block3A,block3B,block3C;	
	switch(block)
	{
	case 0:
		((CEdit*)GetDlgItem(IDC_BLOCK0))->GetWindowText(blockData);  //��ȡ��0������
		break;
	case 1:
		((CEdit*)GetDlgItem(IDC_BLOCK1))->GetWindowText(blockData);  //��ȡ��1������
		break;
	case 2:
		((CEdit*)GetDlgItem(IDC_BLOCK2))->GetWindowText(blockData);  //��ȡ��2������
		break;
	case 3:
		MessageBox("Cannot write block 3!");
		return;
	default:
		break;
	}
	if(blockData.IsEmpty()){//����û�û����������ݣ���������
		MessageBox("Please enter the block data!");
		return;
	}

	if(blockData.GetLength()%2==1)/* ������ݵĳ���Ϊ����������ǰ�油��0 */
		blockData='0'+blockData; 

	// 5.3 �Ի�ȡ�����ݽ��д���
	unsigned char src_data[100];//src_data��д�����Ϣ����
	int src_len;//src_len�� ��Ϣ�ĳ���
	HexStr2CharArray(blockData,src_data,&asc_len);   // ��16����CString��blockDataת��Ϊ�ֽ���unsigned char*
	src_len=asc_len;
	// 5.4 ����ȡ������д����Ӧ�Ŀ���	
	//	��ѡ��Ŀ������ú���write_block�ӿں���
	//	��д��ʧ�ܣ����ڡ�״̬���༭������ʾ��Write block x failed�������ɹ�����ʾ��Write block successfully!��
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
