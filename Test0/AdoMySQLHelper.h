// AdoMySQLHelper.h: interface for the CAdoMySQLHelper class.
//
//////////////////////////////////////////////////////////////////////
#import "./libs/msado15.dll" no_namespace rename("EOF","ADOEOF") rename("BOF","ADOBOF")

#if !defined(AFX_ADOMYSQLHELPER_H__41154AAE_BAEF_42A0_AF4F_A05592515C6D__INCLUDED_)
#define AFX_ADOMYSQLHELPER_H__41154AAE_BAEF_42A0_AF4F_A05592515C6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct RemainTime{
	CString UID;
	int RemainSeconds;
	
	RemainTime(CString uid,int remainSeconds){
		UID = uid;
		RemainSeconds = remainSeconds;
	}
};

struct OnRecord{
	CString UID;
	int RemainSeconds;
	CString StartTime;
	BOOL isOvertime;

	OnRecord(CString uid,int remainSeconds,CString st,BOOL isover){
		UID = uid;
		RemainSeconds = remainSeconds;
		StartTime = st;
		isOvertime = isover;
	};
	
	OnRecord(){
		UID = "";
		RemainSeconds = 0;
		StartTime = "";
		isOvertime = true;
	}

};

//���ݿ������
class CAdoMySQLHelper  
{
public:	
	//�򿪹ر����ݿ�����
	void MySQL_Close();
	bool MySQL_Connect();
	~CAdoMySQLHelper(){
		this->MySQL_Close();
	}	
	bool MySQL_Insert(struct RemainTime record);//��ʱ�����	
	bool MySQL_Insert(struct OnRecord record);	//�ϻ������һ����¼
	bool MySQL_Delete(CString uid,CString table);//����Ϊtable�ı���ɾ��UIDֵΪuid������
	//����ʱ���������
	bool MySQL_UpdateRemainTime(CString uid,int updateTime,CString table);
    //����ʱ���в���UIDΪuid�ļ�¼
    //������ʱ,����ʧ�ܷ���false
	bool MySQL_Query(CString uid,int &rm);
    //���ϻ����в���UIDΪuid�ļ�¼
    //���ֵ ��ʱ/�Ƿ����/��ʼʱ��(��Ϊ�β��޸�) 
    bool MySQL_Query(CString uid,struct OnRecord &or);
	//����true��ʾ���ε��óɹ�  false��ʾ����ʧ��
	bool MySQL_ScanOnTable();
private:
	//�����ӵ�����ָ��
	_ConnectionPtr m_pConnection; 	

};


#endif // !defined(AFX_ADOMYSQLHELPER_H__41154AAE_BAEF_42A0_AF4F_A05592515C6D__INCLUDED_)
