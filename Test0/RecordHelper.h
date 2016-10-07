// RecordHelper.h: interface for the CRecordHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDHELPER_H__BD280CA1_0D9A_4AD2_B14B_B60DDE87CD14__INCLUDED_)
#define AFX_RECORDHELPER_H__BD280CA1_0D9A_4AD2_B14B_B60DDE87CD14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRecordHelper  
{
public:
	//CRecordHelper();
	CRecordHelper(CString SaveFiles="records.txt");
	virtual ~CRecordHelper();

	//钱包记录
	void walletRecord(CString uid,CString accounts,long Remaingst,CString str);
	//void SaveReacharges(CString uid,CString accounts,long Remaingst);
	//void SaveConsumptions(CString uid,CString accounts,long Remaings);

	//上网记录
	void SignInRecord(CString uid,int RemainingSeconds);
	void ExitRecord(CString uid,int RemainingSeconds,int OverSeconds);

	//写到编辑框
	CString LoadRecords(void);

	//清空
	BOOL isEmptyRecord(void);

private:
	CString mSaveFiles;

};


#endif // !defined(AFX_RECORDHELPER_H__BD280CA1_0D9A_4AD2_B14B_B60DDE87CD14__INCLUDED_)
