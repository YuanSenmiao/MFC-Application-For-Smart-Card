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

//数据库操作类
class CAdoMySQLHelper  
{
public:	
	//打开关闭数据库连接
	void MySQL_Close();
	bool MySQL_Connect();
	~CAdoMySQLHelper(){
		this->MySQL_Close();
	}	
	bool MySQL_Insert(struct RemainTime record);//余时表插入	
	bool MySQL_Insert(struct OnRecord record);	//上机表插入一条记录
	bool MySQL_Delete(CString uid,CString table);//在名为table的表中删除UID值为uid的数据
	//在余时表更新数据
	bool MySQL_UpdateRemainTime(CString uid,int updateTime,CString table);
    //在余时表中查找UID为uid的记录
    //返回余时,查找失败返回false
	bool MySQL_Query(CString uid,int &rm);
    //在上机表中查找UID为uid的记录
    //输出值 余时/是否结束/开始时间(作为形参修改) 
    bool MySQL_Query(CString uid,struct OnRecord &or);
	//返回true表示本次调用成功  false表示调用失败
	bool MySQL_ScanOnTable();
private:
	//打开连接的智能指针
	_ConnectionPtr m_pConnection; 	

};


#endif // !defined(AFX_ADOMYSQLHELPER_H__41154AAE_BAEF_42A0_AF4F_A05592515C6D__INCLUDED_)
