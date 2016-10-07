// AdoMySQLHelper.cpp: implementation of the CAdoMySQLHelper class.
//
//////////////////////////////////////////////////////////////////////
#include "./libs/ZM124U.h"
#import "./libs/msado15.dll" no_namespace rename("EOF","ADOEOF") rename("BOF","ADOBOF")
_COM_SMARTPTR_TYPEDEF(_Connection, __uuidof(_Connection));
_COM_SMARTPTR_TYPEDEF(_Recordset, __uuidof(_Recordset));
_COM_SMARTPTR_TYPEDEF(_Command, __uuidof(_Command));

#include "stdafx.h"
#include "Test0.h"
#include "AdoMySQLHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
bool CAdoMySQLHelper::MySQL_Connect()
{
    CoInitialize(NULL); //��ʼ��COM��
	//AfxOleInit();
	//AfxMessageBox("���ӣ�");
	
    try{
		HRESULT hr = this->m_pConnection.CreateInstance("ADODB.Connection");
		//HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
		
		if(FAILED(hr)){
			AfxMessageBox("Creating Connection Pointer error!");
			return false;
		}
		
		//���ó�ʱʱ��
		this->m_pConnection->ConnectionTimeout = 600;
		//�������ʱʱ��
		this->m_pConnection->CommandTimeout = 120;
		
		this->m_pConnection->Open("DSN=mysql;Server=localhost; Database=rfid",  //���ݿ������ַ���
			"root",
			"1234",
			adModeUnknown);
    }
    catch(_com_error &e){
        AfxMessageBox(e.Description());
        return false;
    }
    return true;
	
}

void CAdoMySQLHelper::MySQL_Close()
{
    if((NULL != this->m_pConnection) && (this->m_pConnection->State)){
        this->m_pConnection->Close();       //�ر�����
        this->m_pConnection.Release();      //�ͷ�����
        this->m_pConnection = NULL;
    }
	//������COM�����Ҫ����CoUninitialize����ж��COM��
	CoUninitialize();     
}

bool CAdoMySQLHelper::MySQL_Insert(struct RemainTime record){
	//���ұ�����
	_variant_t table_name = "RemainTimeTable";
	
	try{
		_RecordsetPtr m_pRecordset;
		HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)){
			AfxMessageBox("Create Recordset Pointer error![RemainTimeTable]");
			return false;
		}
		//�����ݿ��еı�
		m_pRecordset->Open(table_name,
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //�α�����
			adLockOptimistic,   
			adCmdTableDirect);  
		//�ж��Ƿ�֧����Ӽ�¼
		if(!m_pRecordset->Supports(adAddNew)){
			AfxMessageBox("Failed to insert into RemainTimeTable");
			return false;
		}
		
		m_pRecordset->AddNew();
		
		//���UID
		_variant_t uid = record.UID;
		m_pRecordset->PutCollect("UID",uid);
		
		//���ʣ��ʱ��
		_variant_t time;
		time.ChangeType(VT_INT);
		time.intVal = record.RemainSeconds;
		m_pRecordset->Fields->GetItem("RemainTime")->Value = time;
		
		m_pRecordset->Update();
		m_pRecordset->Close();
		return true;
		
	}catch(_com_error &e){
		AfxMessageBox(e.Description());
		return false;
	}
}

bool CAdoMySQLHelper::MySQL_Insert(struct OnRecord record){
	//���ұ�����
	_variant_t table_name = "OnlineTable";
	
	try{
		_RecordsetPtr m_pRecordset;
		HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)){
			AfxMessageBox("Create Recordset Pointer error![OnlineTable]");
			return false;
		}
		//�����ݿ��еı�
		m_pRecordset->Open(table_name,
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //�α�����
			adLockOptimistic,   
			adCmdTableDirect); 
		//�ж��Ƿ�֧����Ӽ�¼
		if(!m_pRecordset->Supports(adAddNew)){
			AfxMessageBox("Failed to insert into OnlineTable");
			return false;
		}
		
		m_pRecordset->AddNew();
		
		//���UID
		_variant_t uid = record.UID;
		m_pRecordset->PutCollect("UID",uid);
		
		//���ʣ��ʱ��
		_variant_t time;
		time.ChangeType(VT_INT);
		time.intVal = record.RemainSeconds;
		m_pRecordset->Fields->GetItem("RemainTime")->Value = time;
		
		//����ϻ���ʼʱ��
		_variant_t startTime = record.StartTime;
		m_pRecordset->PutCollect("StartTime",startTime);
		
		
		//����Ƿ�����ϻ�
		_variant_t isover;
		time.ChangeType(VT_BOOL);
		time.boolVal = record.isOvertime? VARIANT_TRUE: VARIANT_FALSE;
		m_pRecordset->Fields->GetItem("isOvertime")->PutValue(isover);
		
		
		m_pRecordset->Update();
		m_pRecordset->Close();
		
		return true;
	}catch(_com_error &e){
		AfxMessageBox(e.Description());
		return false;
	}
}


bool CAdoMySQLHelper::MySQL_UpdateRemainTime(CString uid,int updateTime,CString table){
	_CommandPtr m_pCommand;
	try{
		m_pCommand.CreateInstance("ADODB.Command");
		_variant_t vNULL;
		vNULL.vt = VT_ERROR;
		vNULL.scode = DISP_E_PARAMNOTFOUND;
		m_pCommand->ActiveConnection = this ->m_pConnection;
		
		CString update;
		update.Format("%d",updateTime);
		
		m_pCommand->CommandText = (_bstr_t)("update "+table+" set RemainTime = "  + update + " where UID = \'" + uid + "\'");
		m_pCommand->Execute(&vNULL,&vNULL,adCmdText);
	}catch(_com_error &e){
		m_pCommand.Release();
		AfxMessageBox(e.Description());
		return false;
	}
	return true;
}


bool CAdoMySQLHelper::MySQL_Delete(CString uid,CString table){
	//���ұ�����
	_variant_t sql = "SELECT * FROM "+ table + " where UID=\'" + uid +"\'";
	
	try{
		_RecordsetPtr m_pRecordset;
		HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)){
			AfxMessageBox("Create Recordset Pointer error![Delete]");
			return false;
		}
		//�����ݿ��еı�
		m_pRecordset->Open(sql,
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //�α�����
			adLockOptimistic, 
			adCmdText); 
		//�������ݿ�����
		if(!m_pRecordset->ADOEOF){
			m_pRecordset->MoveFirst(); //�ƶ�����ͷ
			
			while(!m_pRecordset->ADOEOF){
				m_pRecordset->Delete(adAffectCurrent);
				m_pRecordset->Update();
				m_pRecordset->MoveNext();
			}
			m_pRecordset->Close();//�ر�ָ��
		}
		return true;
	}catch(_com_error &e){
		AfxMessageBox(e.Description());
		return false;
	}
}

//���ϻ������UIDΪuid��ֵ
//���ֵ ��ʱ/�Ƿ����/��ʼʱ��
bool CAdoMySQLHelper::MySQL_Query(CString uid,struct OnRecord &or){
	//���ұ�����
	_variant_t table_name = "onlinetable";
	
	try{
		_RecordsetPtr m_pRecordset;
		HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)){
			AfxMessageBox("Create Recordset Pointer error![OnlineTable]");
			return false;
		}
		//�����ݿ��еı�
		m_pRecordset->Open(table_name,
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //�α����ͣ���������������
			adLockOptimistic,   
			adCmdTableDirect);  //ֱ�Ӵ򿪱�
		//�������ݿ�����
		if(!m_pRecordset->ADOEOF){
			m_pRecordset->MoveFirst(); //�ƶ�����ͷ
			while(!m_pRecordset->ADOEOF){
				if (uid == m_pRecordset->Fields->GetItem("UID")->Value.bstrVal ){
                    //��ȡUID
					//or.UID = uid;					
					//��ȡ��ʱ��Ϣ
					_variant_t time = m_pRecordset->Fields->GetItem("RemainTime")->Value;
					if(time.vt != VT_NULL){
						time.ChangeType(VT_INT);
						or.RemainSeconds = time.intVal;
					}
					
					//��ȡ��ʼʱ����Ϣ
					_variant_t st = m_pRecordset->Fields->GetItem("StartTime")->Value;
					if(time.vt != VT_NULL){
						st.ChangeType(VT_BSTR);
						or.StartTime = st.bstrVal;
					}
					
					m_pRecordset->Close();//�ر�ָ��
					return true;
				}
				m_pRecordset->MoveNext();
			}
			m_pRecordset->Close();//�ر�ָ��
		}
		return false;
	}catch(_com_error &e){
		AfxMessageBox(e.Description());
		return false;
	}
}

//������ʱ��
//������ʱ��intֵ
//����ʧ�ܷ���-1
bool CAdoMySQLHelper::MySQL_Query(CString uid,int &rm){
	//���ұ�����
	_variant_t table_name = "RemainTimeTable";
	
	try{
		_RecordsetPtr m_pRecordset;
		HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)){
			AfxMessageBox("Create Recordset Pointer error!(RemainTimeTable)");
			return false;
		}
		//�����ݿ��еı�
		m_pRecordset->Open(table_name,
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //�α����ͣ���������������
			adLockOptimistic,  
			adCmdTableDirect);  //ֱ�Ӵ򿪱�
		//�������ݿ�����
		if(!m_pRecordset->ADOEOF){
			m_pRecordset->MoveFirst(); //�ƶ�����ͷ
			while(!m_pRecordset->ADOEOF){
				if (uid == m_pRecordset->Fields->GetItem("UID")->Value.bstrVal ){
					;
					//��ȡ��ʱ��Ϣ
					_variant_t var = m_pRecordset->Fields->GetItem("RemainTime")->Value;
					var.ChangeType(VT_INT);
					rm = var.intVal;
					
					m_pRecordset->Close();//�ر�ָ��
					return true;
				}
				m_pRecordset->MoveNext();
			}
			m_pRecordset->Close();//�ر�ָ��
		}
		return false;
	}catch(_com_error &e){
		AfxMessageBox(e.Description());
		return false;
	}
	
}

bool CAdoMySQLHelper::MySQL_ScanOnTable(){
	_variant_t sql = "SELECT * FROM onlinetable";
	try{
		_RecordsetPtr m_pRecordset;
		HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
		
		if(FAILED(hr)){
			AfxMessageBox("Create Recordset Pointer error![OnlineTable]");
			return false;
		}
		
		//�����ݿ�onlinetable��
		m_pRecordset->Open(sql, 
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //�α�����
			adLockOptimistic, 
			adCmdText);  //ֱ�Ӵ򿪱�
		
		if(!m_pRecordset->ADOEOF){
			m_pRecordset->MoveFirst(); //�ƶ�����ͷ
			
			//��ȡָ�뵱ǰ���UID
			CString currentUID = (LPCSTR)_bstr_t(m_pRecordset->Fields->GetItem("UID")->Value); 
			while(!m_pRecordset->ADOEOF){
				//����ת��
				_variant_t rmTime;
				rmTime = m_pRecordset->Fields->GetItem("RemainTime")->Value;
				rmTime.ChangeType(VT_INT);				
				//�����ʱ����0
				if(rmTime.intVal > 5){
					rmTime.intVal = rmTime.intVal - 5; //��ʱ����
					m_pRecordset->Fields->GetItem("RemainTime")->Value = rmTime; //���µ�ǰ����
					m_pRecordset->Update(); 
				} 
				else{ //ǿ���˳���������ʱ����ɾ���ϻ�������
					if(!MySQL_UpdateRemainTime(currentUID,0,"RemainTimeTable")){
						AfxMessageBox("Failed to update RemainTimeTable!");//������ʱ��ʧ��
						m_pRecordset->Close();
						return false;
					}					
					if(!MySQL_Delete(currentUID,"onlinetable")){
						AfxMessageBox("Failed to update OnlineTable!");//ɾ���ϻ��û���ʧ��
						m_pRecordset->Close();
						return false;						
					}					
					AfxMessageBox("Sorry! User with UID #" + currentUID + " have been offline! [Time out]");
				}				
				m_pRecordset->MoveNext();
				if(!m_pRecordset->ADOEOF){
					currentUID = (LPCSTR)_bstr_t( m_pRecordset->Fields->GetItem("UID")->Value ); 
				}				
			}
			
		}
		else{
			// AfxMessageBox("[OnlineTable] is empty");				
		}
		
		m_pRecordset->Close();//�ر�ָ��		
		return true;
	}catch(_com_error &e){
		AfxMessageBox(e.Description());
		return false;
	}	
}
