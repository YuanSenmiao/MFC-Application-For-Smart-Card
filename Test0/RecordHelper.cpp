// RecordHelper.cpp: implementation of the CRecordHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Test0.h"
#include "RecordHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecordHelper::CRecordHelper(CString SaveFiles)
{
	this->mSaveFiles = SaveFiles;
}

CRecordHelper::~CRecordHelper()
{

}

BOOL CRecordHelper::isEmptyRecord(){
	CFile file;
	
	bool k = file.Open(this->mSaveFiles,CFile::modeCreate | CFile::modeCreate | CFile::modeReadWrite);
	if (!k) return false;
	file.Close();

	return true;
}

CString CRecordHelper::LoadRecords(){
	CFile file;
	
	bool k = file.Open(this->mSaveFiles,CFile::modeNoTruncate | CFile::modeCreate | CFile::modeReadWrite);
	if (!k) {
		AfxMessageBox("Open file failed!");
		file.Close();
		return "";
	}else{
		CString result = "";
		char* temp = new char[11];
		file.SeekToBegin();

		while (true){
			int k = file.Read(temp,10);
			temp[k] = '\0';

			result += temp;
			if (k < 10) break;
		}

		file.Close();
//		AfxMessageBox(result);
		return result;
	}
}

void CRecordHelper::SignInRecord(CString uid,int RemainingSeconds)
{
	CFile file;
	bool k = file.Open(this->mSaveFiles,CFile::modeNoTruncate | CFile::modeCreate | CFile::modeReadWrite);
	if (!k) {
		AfxMessageBox("Open file failed!");
		file.Close();
		return ;
	}else{
		CTime t = CTime::GetCurrentTime();
		CString s = t.Format("%Y/%m/%d %H:%M:%S");

		CString r = "";
		r += "UID: " + uid + "\r\n";
		r += "Time: " + s + "\r\n";
		if (RemainingSeconds != 0) r += "Status: Sign in successful\r\n";
		else r +=  "Status: No enough remain time! Sign in failed!\r\n";
		r += "Content: User sign in\r\n";
		r += "Remain time: ";

		int hour = RemainingSeconds/3600;
		int minutes = (RemainingSeconds - hour*3600)/60;
		int seconds = RemainingSeconds - hour*3600 - minutes*60;
		CString temp;
		temp.Format("%d h %d m %d s\r\n",hour,minutes,seconds);
		r += temp;
		r += "\r\n\r\n";

		char* tempx = new char[11];
		file.SeekToBegin();

		while (true){
			int k = file.Read(tempx,10);
			tempx[k] = '\0';

			r += tempx;
			if (k < 10) break;
		}
		
		file.SeekToBegin();
		file.Write(r,r.GetLength());
		file.Close();
	}

}

void CRecordHelper::ExitRecord(CString uid,int RemainingSeconds,int OverSeconds)
{
	CFile file;
	bool k = file.Open(this->mSaveFiles,CFile::modeNoTruncate | CFile::modeCreate | CFile::modeReadWrite);
	if (!k) {
		AfxMessageBox("Open file failed!");
		file.Close();
		return ;
	}else{
		CTime t = CTime::GetCurrentTime();
		CString s = t.Format("%Y/%m/%d %H:%M:%S");

		CString r = "";
		r += "UID: " + uid + "\r\n";
		r += "Time: " + s + "\r\n";
		r += "Status: Exit successfully";
		if (RemainingSeconds < 60*30) r += " (Attention: Remain time is less than 30 minutes)\r\n";
		else r += "\r\n";

		CString temp;
		int hour = OverSeconds/3600;
		int minutes = (OverSeconds - hour*3600)/60;
		int seconds = OverSeconds - hour*3600 - minutes*60;
		temp.Format("%d h %d m %d s\r\n",hour,minutes,seconds);
		r += "Content: User exit, online time: " + temp;
		r += "Remain time: ";

		hour = RemainingSeconds/3600;
		minutes = (RemainingSeconds - hour*3600)/60;
		seconds = RemainingSeconds - hour*3600 - minutes*60;
		temp.Format("%d h %d m %d s\r\n",hour,minutes,seconds);
		r += temp;
		r += "\r\n\r\n";

		char* tempx = new char[11];
		file.SeekToBegin();

		while (true){
			int k = file.Read(tempx,10);
			tempx[k] = '\0';

			r += tempx;
			if (k < 10) break;
		}
		
		file.SeekToBegin();
		file.Write(r,r.GetLength());
		file.Close();
	}
}

void CRecordHelper::walletRecord(CString uid,CString accounts,long Remaings,CString str)
{
CFile file;
	bool k = file.Open(this->mSaveFiles,CFile::modeNoTruncate | CFile::modeCreate | CFile::modeReadWrite);
	if (!k) {
		AfxMessageBox("Open file failed!");
		file.Close();
		return ;
	}else{
		CTime t = CTime::GetCurrentTime();
		CString s = t.Format("%Y/%m/%d %H:%M:%S");
		CString r = "";
		r += "UID: " + uid + "\r\n";
		r += str+" Money: "+accounts+"\r\n";
		//r += "充值金额: "+accounts+"\r\n";
		r += "Status: "+str+" successfully!\r\n";
		//r += "状态: 充值成功！\r\n";

		CString temp;
		temp.Format("%d\r\n",Remaings);
		r += "Balance: " + temp;
		r += "Time: " + s;
		r += "\r\n\r\n";
		
		char* tempx = new char[11];
		file.SeekToBegin();

		while (true){
			int k = file.Read(tempx,10);
			tempx[k] = '\0';

			r += tempx;
			if (k < 10) break;
		}
		
		file.SeekToBegin();
		file.Write(r,r.GetLength());
		file.Close();
	}
}