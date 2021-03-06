
// CircleTraderMFCDlg.h: 头文件
//

#pragma once
#include "CustomMdSpi.h"
#include "CustomTradeSpi.h"
#include "mysqlinterface.h"
#include "Strategy1.h"
#include <iostream>
#include <fstream>
#include "InstrumentsMap.h"
#include <map>
#include <list>
using namespace std;

#define TABLE_FILENAME "MatchTable.txt"
#define VIEWHISTORYINFO "ViewHistoryInfo.txt"
// CCircleTraderMFCDlg 对话框
class CCircleTraderMFCDlg : public CDialogEx
{
	
// 构造
public:
	CCircleTraderMFCDlg(CWnd* pParent = nullptr);	// 标准构造函数
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CIRCLETRADERMFC_DIALOG };
#endif
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	HANDLE hProcess;
	HANDLE hThread;
	bool closeFlag;
	InstrumentsMap instrusMap;
	list<int> shanghaiInstru;
	list<int> dalianInstru;
	list<int> zhengzhouInstru;
	list<pair<int, string>> HistoryInfoList;
	map<int, int> IDMap;
public:
	CString shanghaiCuEdit;
	CString shanghaiAlEdit;
	CString shanghaiZnEdit;
	CString shanghaiNiEdit;
	CString shanghaiAuEdit;
	CString shanghaiAgEdit;
	CString shanghaiRbEdit;
	CString shanghaiHcEdit;
	CString shanghaiFuEdit;
	CString shanghaiBuEdit;
	CString shanghaiRuEdit;
	CString shanghaiScEdit;
	CString daLianMEdit;
	CString daLianYEdit;
	CString daLianAEdit;
	CString daLianPEdit;
	CString daLianCEdit;
	CString daLianCsEdit;
	CString daLianJdEdit;
	CString daLianLEdit;
	CString daLianVEdit;
	CString daLianPPEdit;
	CString daLianJEdit;
	CString daLianJmEdit;
	CString daLianIEdit;
	CString zhengzhouSrEdit;
	CString zhengzhouCfEdit;
	CString zhengzhouZcEdit;
	CString zhengzhouTaEdit;
	CString zhengzhouMaEdit;
	CString zhengzhouOiEdit;
	CString zhengzhouRmEdit;
	CString zhengzhouSfEdit;
	CString zhengzhouSmEdit;
	CString zhengzhouApEdit;
	CStringList instruList;
	void CheckedGetInstru(int checkID, int editID, CString& instru);
	afx_msg void OnBnClickedStarttradebtn();
	afx_msg void OnBnClickedButton1();
	void SetInstruToView(int editID, string instru);
	void ReadHistoryInstruments(list<pair<int, string>> &instrus, string filename);
	void WriteHistoryInstruments(list<pair<int, string>> historyInfo, string filename);
	void SetIdMap();
};
