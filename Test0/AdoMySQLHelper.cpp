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
    CoInitialize(NULL); //初始化COM库
	//AfxOleInit();
	//AfxMessageBox("连接！");
	
    try{
		HRESULT hr = this->m_pConnection.CreateInstance("ADODB.Connection");
		//HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
		
		if(FAILED(hr)){
			AfxMessageBox("Creating Connection Pointer error!");
			return false;
		}
		
		//设置超时时间
		this->m_pConnection->ConnectionTimeout = 600;
		//设置命令超时时间
		this->m_pConnection->CommandTimeout = 120;
		
		this->m_pConnection->Open("DSN=mysql;Server=localhost; Database=rfid",  //数据库连接字符串
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
        this->m_pConnection->Close();       //关闭连接
        this->m_pConnection.Release();      //释放连接
        this->m_pConnection = NULL;
    }
	//访问完COM库后，需要调用CoUninitialize函数卸载COM库
	CoUninitialize();     
}

bool CAdoMySQLHelper::MySQL_Insert(struct RemainTime record){
	//查找表名称
	_variant_t table_name = "RemainTimeTable";
	
	try{
		_RecordsetPtr m_pRecordset;
		HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)){
			AfxMessageBox("Create Recordset Pointer error![RemainTimeTable]");
			return false;
		}
		//打开数据库中的表
		m_pRecordset->Open(table_name,
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //游标类型
			adLockOptimistic,   
			adCmdTableDirect);  
		//判断是否支持添加记录
		if(!m_pRecordset->Supports(adAddNew)){
			AfxMessageBox("Failed to insert into RemainTimeTable");
			return false;
		}
		
		m_pRecordset->AddNew();
		
		//添加UID
		_variant_t uid = record.UID;
		m_pRecordset->PutCollect("UID",uid);
		
		//添加剩余时间
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
	//查找表名称
	_variant_t table_name = "OnlineTable";
	
	try{
		_RecordsetPtr m_pRecordset;
		HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)){
			AfxMessageBox("Create Recordset Pointer error![OnlineTable]");
			return false;
		}
		//打开数据库中的表
		m_pRecordset->Open(table_name,
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //游标类型
			adLockOptimistic,   
			adCmdTableDirect); 
		//判断是否支持添加记录
		if(!m_pRecordset->Supports(adAddNew)){
			AfxMessageBox("Failed to insert into OnlineTable");
			return false;
		}
		
		m_pRecordset->AddNew();
		
		//添加UID
		_variant_t uid = record.UID;
		m_pRecordset->PutCollect("UID",uid);
		
		//添加剩余时间
		_variant_t time;
		time.ChangeType(VT_INT);
		time.intVal = record.RemainSeconds;
		m_pRecordset->Fields->GetItem("RemainTime")->Value = time;
		
		//添加上机开始时间
		_variant_t startTime = record.StartTime;
		m_pRecordset->PutCollect("StartTime",startTime);
		
		
		//添加是否结束上机
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
	//查找表名称
	_variant_t sql = "SELECT * FROM "+ table + " where UID=\'" + uid +"\'";
	
	try{
		_RecordsetPtr m_pRecordset;
		HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)){
			AfxMessageBox("Create Recordset Pointer error![Delete]");
			return false;
		}
		//打开数据库中的表
		m_pRecordset->Open(sql,
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //游标类型
			adLockOptimistic, 
			adCmdText); 
		//搜索数据库内容
		if(!m_pRecordset->ADOEOF){
			m_pRecordset->MoveFirst(); //移动到开头
			
			while(!m_pRecordset->ADOEOF){
				m_pRecordset->Delete(adAffectCurrent);
				m_pRecordset->Update();
				m_pRecordset->MoveNext();
			}
			m_pRecordset->Close();//关闭指针
		}
		return true;
	}catch(_com_error &e){
		AfxMessageBox(e.Description());
		return false;
	}
}

//在上机表查找UID为uid的值
//输出值 余时/是否结束/开始时间
bool CAdoMySQLHelper::MySQL_Query(CString uid,struct OnRecord &or){
	//查找表名称
	_variant_t table_name = "onlinetable";
	
	try{
		_RecordsetPtr m_pRecordset;
		HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)){
			AfxMessageBox("Create Recordset Pointer error![OnlineTable]");
			return false;
		}
		//打开数据库中的表
		m_pRecordset->Open(table_name,
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //游标类型：单纯的向下搜索
			adLockOptimistic,   
			adCmdTableDirect);  //直接打开表
		//搜索数据库内容
		if(!m_pRecordset->ADOEOF){
			m_pRecordset->MoveFirst(); //移动到开头
			while(!m_pRecordset->ADOEOF){
				if (uid == m_pRecordset->Fields->GetItem("UID")->Value.bstrVal ){
                    //获取UID
					//or.UID = uid;					
					//获取余时信息
					_variant_t time = m_pRecordset->Fields->GetItem("RemainTime")->Value;
					if(time.vt != VT_NULL){
						time.ChangeType(VT_INT);
						or.RemainSeconds = time.intVal;
					}
					
					//获取开始时间信息
					_variant_t st = m_pRecordset->Fields->GetItem("StartTime")->Value;
					if(time.vt != VT_NULL){
						st.ChangeType(VT_BSTR);
						or.StartTime = st.bstrVal;
					}
					
					m_pRecordset->Close();//关闭指针
					return true;
				}
				m_pRecordset->MoveNext();
			}
			m_pRecordset->Close();//关闭指针
		}
		return false;
	}catch(_com_error &e){
		AfxMessageBox(e.Description());
		return false;
	}
}

//查找余时表
//返回余时的int值
//查找失败返回-1
bool CAdoMySQLHelper::MySQL_Query(CString uid,int &rm){
	//查找表名称
	_variant_t table_name = "RemainTimeTable";
	
	try{
		_RecordsetPtr m_pRecordset;
		HRESULT hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
		if(FAILED(hr)){
			AfxMessageBox("Create Recordset Pointer error!(RemainTimeTable)");
			return false;
		}
		//打开数据库中的表
		m_pRecordset->Open(table_name,
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //游标类型：单纯的向下搜索
			adLockOptimistic,  
			adCmdTableDirect);  //直接打开表
		//搜索数据库内容
		if(!m_pRecordset->ADOEOF){
			m_pRecordset->MoveFirst(); //移动到开头
			while(!m_pRecordset->ADOEOF){
				if (uid == m_pRecordset->Fields->GetItem("UID")->Value.bstrVal ){
					;
					//获取余时信息
					_variant_t var = m_pRecordset->Fields->GetItem("RemainTime")->Value;
					var.ChangeType(VT_INT);
					rm = var.intVal;
					
					m_pRecordset->Close();//关闭指针
					return true;
				}
				m_pRecordset->MoveNext();
			}
			m_pRecordset->Close();//关闭指针
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
		
		//打开数据库onlinetable表
		m_pRecordset->Open(sql, 
			_variant_t((IDispatch*)(this->m_pConnection),true),
			adOpenForwardOnly,  //游标类型
			adLockOptimistic, 
			adCmdText);  //直接打开表
		
		if(!m_pRecordset->ADOEOF){
			m_pRecordset->MoveFirst(); //移动到开头
			
			//获取指针当前项的UID
			CString currentUID = (LPCSTR)_bstr_t(m_pRecordset->Fields->GetItem("UID")->Value); 
			while(!m_pRecordset->ADOEOF){
				//类型转换
				_variant_t rmTime;
				rmTime = m_pRecordset->Fields->GetItem("RemainTime")->Value;
				rmTime.ChangeType(VT_INT);				
				//如果余时大于0
				if(rmTime.intVal > 5){
					rmTime.intVal = rmTime.intVal - 5; //余时更新
					m_pRecordset->Fields->GetItem("RemainTime")->Value = rmTime; //更新当前数据
					m_pRecordset->Update(); 
				} 
				else{ //强制退出：更新余时表并且删除上机表内容
					if(!MySQL_UpdateRemainTime(currentUID,0,"RemainTimeTable")){
						AfxMessageBox("Failed to update RemainTimeTable!");//更新余时表失败
						m_pRecordset->Close();
						return false;
					}					
					if(!MySQL_Delete(currentUID,"onlinetable")){
						AfxMessageBox("Failed to update OnlineTable!");//删除上机用户表失败
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
		
		m_pRecordset->Close();//关闭指针		
		return true;
	}catch(_com_error &e){
		AfxMessageBox(e.Description());
		return false;
	}	
}
