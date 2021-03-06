
// CircleTraderMFCDlg.cpp: 实现文件
//

#include "stdafx.h"
#pragma warning(disable: 4996)
#include "CircleTraderMFC.h"
#include "CircleTraderMFCDlg.h"
#include "InstrumentsMap.h"
#include "afxdialogex.h"
#include <ostream>
#include <string>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

TThostFtdcBrokerIDType gBrokerID;
TThostFtdcInvestorIDType gInvesterID;
TThostFtdcPasswordType gInvesterPassword;
//行情参数
FrontAddress gMdFrontAddr;
TThostFtdcInstrumentIDType g_pInstrumentID[] = { "i1809" };
int instrumentNum = 0;
// 交易参数
FrontAddress gTradeFrontAddr;
TThostFtdcInstrumentIDType g_pTradeInstrumentID2;

map<string, CustomTradeSpi *> m_trade;
HANDLE hMutex = CreateMutex(NULL, false, NULL);
Strategy1 *strate = NULL;
CustomMdSpi *market = NULL;
bool readAccountConf();
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCircleTraderMFCDlg 对话框



CCircleTraderMFCDlg::CCircleTraderMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CIRCLETRADERMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCircleTraderMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, ShangHaiCuEdit, shanghaiCuEdit);
	DDX_Text(pDX, ShangHaiAlEdit, shanghaiAlEdit);
	DDX_Text(pDX, ShangHaiZnEdit, shanghaiZnEdit);
	DDX_Text(pDX, ShangHaiNiEdit, shanghaiNiEdit);
	DDX_Text(pDX, ShangHaiAuEdit, shanghaiAuEdit);
	DDX_Text(pDX, ShangHaiagEdit, shanghaiAgEdit);
	DDX_Text(pDX, ShangHaiRbEdit, shanghaiRbEdit);
	DDX_Text(pDX, ShangHaiHcEdit, shanghaiHcEdit);
	DDX_Text(pDX, ShangHaiFuEdit, shanghaiFuEdit);
	DDX_Text(pDX, ShangHaiBuEdit, shanghaiBuEdit);
	DDX_Text(pDX, ShangHaiRuEdit, shanghaiRuEdit);
	DDX_Text(pDX, ShangHaiScEdit, shanghaiScEdit);
	DDX_Text(pDX, DaLianMEdit, daLianMEdit);
	DDX_Text(pDX, DaLianYEdit, daLianYEdit);
	DDX_Text(pDX, DaLianAEdit, daLianAEdit);
	DDX_Text(pDX, DaLianPEdit, daLianPEdit);
	DDX_Text(pDX, DaLianCEdit, daLianCEdit);
	DDX_Text(pDX, DaLianCSEdit, daLianCsEdit);
	DDX_Text(pDX, DaLianJdEdit, daLianJdEdit);
	DDX_Text(pDX, DaLianLEdit, daLianLEdit);
	DDX_Text(pDX, DaLianVEdit, daLianVEdit);
	DDX_Text(pDX, DaLianPPEdit, daLianPPEdit);
	DDX_Text(pDX, DaLianJEdit, daLianJEdit);
	DDX_Text(pDX, DaLianJmEdit, daLianJmEdit);
	DDX_Text(pDX, DaLianIEdit, daLianIEdit);
	DDX_Text(pDX, ZhengZhouSrEdit, zhengzhouSrEdit);
	DDX_Text(pDX, ZhengZhouCfEdit, zhengzhouCfEdit);
	DDX_Text(pDX, ZhengZhouZcEdit, zhengzhouZcEdit);
	DDX_Text(pDX, ZhengZhouTaEdit, zhengzhouTaEdit);
	DDX_Text(pDX, ZhengZhouMaEdit, zhengzhouMaEdit);
	DDX_Text(pDX, ZhengZhouOiEdit, zhengzhouOiEdit);
	DDX_Text(pDX, ZhengZhouRmEdit, zhengzhouRmEdit);
	DDX_Text(pDX, ZhengZhouSfEdit, zhengzhouSfEdit);
	DDX_Text(pDX, ZhengZhouSmEdit, zhengzhouSmEdit);
	DDX_Text(pDX, ZhengZhouApEdit, zhengzhouApEdit);
	
}

BEGIN_MESSAGE_MAP(CCircleTraderMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(STARTTRADEBTN, &CCircleTraderMFCDlg::OnBnClickedStarttradebtn)
	ON_BN_CLICKED(IDC_BUTTON1, &CCircleTraderMFCDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CCircleTraderMFCDlg 消息处理程序

BOOL CCircleTraderMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	strate = new Strategy1();
	closeFlag = false;
	//读取对应表
	instrusMap.ReadFileTable(TABLE_FILENAME);
	//读取用户配置文件
	readAccountConf();
	//设置IDMap
	SetIdMap();
	//读取历史界面数据，并显示
	list<pair<int, string>> viewinfoList;
	ReadHistoryInstruments(viewinfoList, VIEWHISTORYINFO);
	if (!viewinfoList.empty())
	{
		list<pair<int, string>>::iterator lit = viewinfoList.begin();
		for (; lit != viewinfoList.end(); ++lit)
		{
			map<int, int>::iterator mit = IDMap.begin();
			for (; mit != IDMap.end(); ++mit)
			{
				if(lit->first == mit->second)
				{
					CButton* pBtn = (CButton*)GetDlgItem(mit->first);
					pBtn->SetCheck(1);
				}
			}
			SetInstruToView(lit->first, lit->second);
		}
		UpdateData(FALSE);
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCircleTraderMFCDlg::ReadHistoryInstruments(list<pair<int, string>>& instrus, string filename)
{
	ifstream ifs;
	ifs.open(filename);
	string instruInfo;
	string instru;
	while (ifs >> instruInfo)
	{
		InstrumentsMap map;
		vector<string> vectInstru = map.split(instruInfo, "&&");
		pair<int, string> tmp;
		tmp.first = atoi(vectInstru.at(0).c_str());
		tmp.second = vectInstru.at(1);
		instrus.push_back(tmp);
	}
}

void CCircleTraderMFCDlg::WriteHistoryInstruments(list<pair<int, string>> historyInfo, string filename)
{
	ofstream ofs;
	ofs.open(filename);
	list<pair<int, string>>::iterator it = historyInfo.begin();
	for (; it != historyInfo.end(); ++it)
	{
		ofs << it->first << "&&" << it->second << endl;
	}
}

void CCircleTraderMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCircleTraderMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCircleTraderMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool readAccountConf()
{
	ifstream ifs;
	ifs.open("account.ini");

	string str;
	if (ifs >> str)
	{
		cout << "gBrokerID: " << str << endl;
		strcpy_s(gBrokerID, strlen(str.data()) + 1, str.data());
		ifs >> str;
		cout << "gInvesterID: " << str << endl;
		strcpy_s(gInvesterID, strlen(str.data()) + 1, str.data());
		ifs >> str;
		strcpy_s(gInvesterPassword, strlen(str.data()) + 1, str.data());
		ifs >> str;
		strcpy_s(gTradeFrontAddr, strlen(str.data()) + 1, str.data());
		ifs >> str;
		strcpy_s(gMdFrontAddr, strlen(str.data()) + 1, str.data());
	}
	ifs.close();
	return false;
}

DWORD WINAPI MProcess(LPVOID lpParamter)
{
	POSITION position;
	CStringList *list = (CStringList*)lpParamter;
	CString instru;
	for (int i = 0; i < list->GetSize(); ++i)
	{

		position = list->FindIndex(i);
		instru = list->GetAt(position);
		string  A = CStringA(instru);
		const char *tmp = A.c_str();
		CustomTradeSpi *trade = new CustomTradeSpi(gBrokerID,
			gInvesterID,
			gInvesterPassword,
			gTradeFrontAddr,
			tmp,
			strate);
		trade->connectTrade();
		m_trade[tmp] = trade;
		//trade = NULL;
	}
	
	char** instrus = (char**)malloc(sizeof(char *)*m_trade.size());
	memset(instrus, 0, sizeof(char *)*m_trade.size());
	map<string, CustomTradeSpi *>::iterator iter = m_trade.begin();
	instrumentNum = m_trade.size();
	for (int i = 0; i < m_trade.size(); i++)
	{
		instrus[i] = (char *)malloc(sizeof(char) * 31);
		strcpy_s(instrus[i], strlen(iter->first.data()) + 1, iter->first.data());
		cout << "==================" << instrus[i] << endl;
		iter++;
	}

	market = new CustomMdSpi(gBrokerID,
		gInvesterID,
		gInvesterPassword,
		gMdFrontAddr,
		instrus,
		m_trade.size(),
		strate);

	market->connectMarket();

	strate->setTradeSpi(m_trade);

	free(instrus);
	instrus = NULL;
	return 0;
}
DWORD WINAPI MThread(LPVOID lpParamter)
{
	ifstream ifs;
	ofstream ofs;
	InstrumentsMap* instrusMap = (InstrumentsMap*)(lpParamter);
	while (1)
	{
		int i = 0;
		for (i = 0; i < 16; ++i)
		{
			string ins, direction, offset, volumn, str;
			char filename[100] = { 0 };

			if (i == 0)
				strcpy_s(filename, strlen("D:/jy/NI/work.ini") + 1, "D:/jy/NI/work.ini");
			else
				sprintf_s(filename, "D:/jy/NI/work%d.ini", i);

			ifs.open(filename);
			ifs >> str;
			if (ifs >> ins)
			{
				string newStru;
				string oldStru = ins.substr(7);
				ifs >> direction;
				ifs >> offset;
				ifs >> volumn;
				int isbuy = atoi(direction.data() + 4);
				int close_open = atoi(offset.data() + 9);
				int volume = atoi(volumn.data() + 10);
				
				//替换VB信号里的合约，和平台相匹配
				instrusMap->ReplaceOldInstru(oldStru, newStru);
				cout << newStru.data() << "-------------------" << endl;
				cout << direction.data() + 4 << "--------------" << endl;
				cout << offset.data() + 9 << "--------------" << endl;
				cout << volumn.data() + 10 << "--------------" << endl;


				if (close_open == 0)
				{
					if (isbuy == 0)
					{
						strate->addWork(newStru, true, true, volume);
					}
					else
					{
						strate->addWork(newStru, true, false, volume);
					}
				}
				else
				{
					if (isbuy == 0)
					{
						strate->addWork(newStru, false, true, volume);
					}
					else
					{
						strate->addWork(newStru, false, false, volume);
					}
				}

				ifs.close();
				ofs.open(filename);
				ofs.close();
			}
			else
			{
				ifs.close();
			}
			Sleep(5);
		}
	}

	return 0;
}

//获取界面被勾选的合约
void CCircleTraderMFCDlg::CheckedGetInstru(int checkID, int editID,CString& instru)
{
 	CWnd* hWnd = (CWnd*)GetDlgItem(checkID);
	if (BST_CHECKED == IsDlgButtonChecked(checkID))
	{
		if (!instru.IsEmpty())
		{
			instruList.AddHead(instru);
			pair<int, string> tmp;
			string  A = CStringA(instru);
			tmp.first = editID;
			tmp.second = A;
			HistoryInfoList.push_back(tmp);
		}
	}
}

void CCircleTraderMFCDlg::SetInstruToView(int editID, string instru)
{
	switch (editID)
	{
	case ShangHaiCuEdit:
		shanghaiCuEdit = (CString)instru.c_str();
		break;
	case ShangHaiAlEdit:
		shanghaiAlEdit = (CString)instru.c_str();
		break;
	case ShangHaiZnEdit:
		shanghaiZnEdit = (CString)instru.c_str();
		break;
	case ShangHaiNiEdit:
		shanghaiNiEdit = (CString)instru.c_str();
		break;
	case ShangHaiAuEdit:
		shanghaiAuEdit = (CString)instru.c_str();
		break;
	case ShangHaiagEdit:
		shanghaiAgEdit = (CString)instru.c_str();
		break;
	case ShangHaiRbEdit:
		shanghaiRbEdit = (CString)instru.c_str();
		break;
	case ShangHaiHcEdit:
		shanghaiHcEdit = (CString)instru.c_str();
		break;
	case ShangHaiFuEdit:
		shanghaiFuEdit = (CString)instru.c_str();
		break;
	case ShangHaiBuEdit:
		shanghaiBuEdit = (CString)instru.c_str();
		break;
	case ShangHaiRuEdit:
		shanghaiRuEdit = (CString)instru.c_str();
		break;
	case ShangHaiScEdit:
		shanghaiScEdit = (CString)instru.c_str();
		break;
	case DaLianMEdit:
		daLianMEdit = (CString)instru.c_str();
		break;
	case DaLianYEdit:
		daLianYEdit = (CString)instru.c_str();
		break;
	case DaLianAEdit:
		daLianAEdit = (CString)instru.c_str();
		break;
	case DaLianPEdit:
		daLianPEdit = (CString)instru.c_str();
		break;
	case DaLianCEdit:
		daLianCEdit = (CString)instru.c_str();
		break;
	case DaLianCSEdit:
		daLianCsEdit = (CString)instru.c_str();
		break;
	case DaLianJdEdit:
		daLianJdEdit = (CString)instru.c_str();
		break;
	case DaLianLEdit:
		daLianLEdit = (CString)instru.c_str();
		break;
	case DaLianVEdit:
		daLianVEdit = (CString)instru.c_str();
		break;
	case DaLianPPEdit:
		daLianPPEdit = (CString)instru.c_str();
		break;
	case DaLianJEdit:
		daLianJEdit = (CString)instru.c_str();
		break;
	case DaLianJmEdit:
		daLianJmEdit = (CString)instru.c_str();
		break;
	case DaLianIEdit:
		daLianIEdit = (CString)instru.c_str();
		break;
	case ZhengZhouSrEdit:
		zhengzhouSrEdit = (CString)instru.c_str();
		break;
	case ZhengZhouCfEdit:
		zhengzhouCfEdit = (CString)instru.c_str();
		break;
	case ZhengZhouZcEdit:
		zhengzhouZcEdit = (CString)instru.c_str();
		break;
	case ZhengZhouTaEdit:
		zhengzhouTaEdit = (CString)instru.c_str();
		break;
	case ZhengZhouMaEdit:
		zhengzhouMaEdit = (CString)instru.c_str();
		break;
	case ZhengZhouOiEdit:
		zhengzhouOiEdit = (CString)instru.c_str();
		break;
	case ZhengZhouRmEdit:
		zhengzhouRmEdit = (CString)instru.c_str();
		break;
	case ZhengZhouSfEdit:
		zhengzhouSfEdit = (CString)instru.c_str();
		break;
	case ZhengZhouSmEdit:
		zhengzhouSmEdit = (CString)instru.c_str();
		break;
	case ZhengZhouApEdit:
		zhengzhouApEdit = (CString)instru.c_str();
		break;
	default:
		break;
	}
}

void CCircleTraderMFCDlg::SetIdMap()
{
	IDMap[ShanghaiCu] = ShangHaiCuEdit;
	IDMap[ShanghaiAl] = ShangHaiAlEdit;
	IDMap[ShanghaiZn] = ShangHaiZnEdit;
	IDMap[ShanghaiNi] = ShangHaiNiEdit;
	IDMap[ShanghaiAu] = ShangHaiAuEdit;
	IDMap[ShanghaiAg] = ShangHaiagEdit;
	IDMap[Shanghairb] = ShangHaiRbEdit;
	IDMap[ShanghaiHc] = ShangHaiHcEdit;
	IDMap[ShanghaiFu] = ShangHaiFuEdit;
	IDMap[ShanghaiBu] = ShangHaiBuEdit;
	IDMap[ShanghaiRu] = ShangHaiRuEdit;
	IDMap[ShanghaiSc] = ShangHaiScEdit;
	IDMap[DaLianM] = DaLianMEdit;
	IDMap[DaLianY] = DaLianYEdit;
	IDMap[DaLianA] = DaLianAEdit;
	IDMap[DaLianP] = DaLianPEdit;
	IDMap[DaLianC] = DaLianCEdit;
	IDMap[DaLianCS] = DaLianCSEdit;
	IDMap[DaLianJd] = DaLianJdEdit;
	IDMap[DaLianL] = DaLianLEdit;
	IDMap[DaLianV] = DaLianVEdit;
	IDMap[DaLianPP] = DaLianPPEdit;
	IDMap[DaLianJ] = DaLianJEdit;
	IDMap[DaLianJm] = DaLianJmEdit;
	IDMap[DaLianI] = DaLianIEdit;
	IDMap[ZhengZhouSr] = ZhengZhouSrEdit;
	IDMap[ZhengZhouCf] = ZhengZhouCfEdit;
	IDMap[ZhengZhouZc] = ZhengZhouZcEdit;
	IDMap[ZhengZhouTa] = ZhengZhouTaEdit;
	IDMap[ZhengZhouMa] = ZhengZhouMaEdit;
	IDMap[ZhengZhouOi] = ZhengZhouOiEdit;
	IDMap[ZhengZhouRm] = ZhengZhouRmEdit;
	IDMap[ZhengZhouSf] = ZhengZhouSfEdit;
	IDMap[ZhengZhouSm] = ZhengZhouSmEdit;
	IDMap[ZhengZhouAp] = ZhengZhouApEdit;
}
//开始交易
void CCircleTraderMFCDlg::OnBnClickedStarttradebtn()
{

	CString tipMsg;
	if (!strcmp(gBrokerID, "1008"))
	{
		tipMsg = "虚盘";
	}
	else
	{
		tipMsg = "实盘";
	}

	int tmp = MessageBox(TEXT("确定要进行") + tipMsg + TEXT("交易吗？") ,TEXT("开始交易"), MB_OKCANCEL);
	if (tmp == 1)
	{
		cout << "开始交易-----------------------" << endl;
		HistoryInfoList.clear();
		instruList.RemoveAll();
		UpdateData(TRUE);
		//获取合约行情
		{
			CheckedGetInstru(ShanghaiCu, ShangHaiCuEdit, shanghaiCuEdit);
			CheckedGetInstru(ShanghaiAl, ShangHaiAlEdit, shanghaiAlEdit);
			CheckedGetInstru(ShanghaiZn, ShangHaiZnEdit, shanghaiZnEdit);
			CheckedGetInstru(ShanghaiNi, ShangHaiNiEdit, shanghaiNiEdit);
			CheckedGetInstru(ShanghaiAu, ShangHaiAuEdit, shanghaiAuEdit);
			CheckedGetInstru(ShanghaiAg, ShangHaiagEdit, shanghaiAgEdit);
			CheckedGetInstru(Shanghairb, ShangHaiRbEdit, shanghaiRbEdit);
			CheckedGetInstru(ShanghaiHc, ShangHaiHcEdit, shanghaiHcEdit);
			CheckedGetInstru(ShanghaiFu, ShangHaiFuEdit, shanghaiFuEdit);
			CheckedGetInstru(ShanghaiBu, ShangHaiBuEdit, shanghaiBuEdit);
			CheckedGetInstru(ShanghaiRu, ShangHaiRuEdit, shanghaiRuEdit);
			CheckedGetInstru(ShanghaiSc, ShangHaiScEdit, shanghaiScEdit);
			CheckedGetInstru(DaLianM, DaLianMEdit, daLianMEdit);
			CheckedGetInstru(DaLianY, DaLianYEdit, daLianYEdit);
			CheckedGetInstru(DaLianA, DaLianAEdit, daLianAEdit);
			CheckedGetInstru(DaLianP, DaLianPEdit, daLianPEdit);
			CheckedGetInstru(DaLianC, DaLianCEdit, daLianCEdit);
			CheckedGetInstru(DaLianCS, DaLianCSEdit, daLianCsEdit);
			CheckedGetInstru(DaLianJd, DaLianJdEdit, daLianJdEdit);
			CheckedGetInstru(DaLianL, DaLianLEdit, daLianLEdit);
			CheckedGetInstru(DaLianV, DaLianVEdit, daLianVEdit);
			CheckedGetInstru(DaLianPP, DaLianPPEdit, daLianPPEdit);
			CheckedGetInstru(DaLianJ, DaLianJEdit, daLianJEdit);
			CheckedGetInstru(DaLianJm, DaLianJmEdit, daLianJmEdit);
			CheckedGetInstru(DaLianI, DaLianIEdit, daLianIEdit);
			CheckedGetInstru(ZhengZhouSr, ZhengZhouSrEdit, zhengzhouSrEdit);
			CheckedGetInstru(ZhengZhouCf, ZhengZhouCfEdit, zhengzhouCfEdit);
			CheckedGetInstru(ZhengZhouZc, ZhengZhouZcEdit, zhengzhouZcEdit);
			CheckedGetInstru(ZhengZhouTa, ZhengZhouTaEdit, zhengzhouTaEdit);
			CheckedGetInstru(ZhengZhouMa, ZhengZhouMaEdit, zhengzhouMaEdit);
			CheckedGetInstru(ZhengZhouOi, ZhengZhouOiEdit, zhengzhouOiEdit);
			CheckedGetInstru(ZhengZhouRm, ZhengZhouRmEdit, zhengzhouRmEdit);
			CheckedGetInstru(ZhengZhouSf, ZhengZhouSfEdit, zhengzhouSfEdit);
			CheckedGetInstru(ZhengZhouSm, ZhengZhouSmEdit, zhengzhouSmEdit);
			CheckedGetInstru(ZhengZhouAp, ZhengZhouApEdit, zhengzhouApEdit);
		}
		
		//清空文件
		ofstream ofs(VIEWHISTORYINFO);
		ofs.close();

		//写入历史文件
		WriteHistoryInstruments(HistoryInfoList, VIEWHISTORYINFO);
		
		//清理内存， 重新开启交易
		if (hProcess && hThread)
		{
			CloseHandle(hProcess);
			CloseHandle(hThread);
			hProcess = NULL;
			hThread = NULL;
			//重新获取合约，创建交易实例
			market->ReleaseUserApi();
			map<string, CustomTradeSpi*>::iterator it = m_trade.begin();
			for (; it != m_trade.end(); ++it)
			{
				CustomTradeSpi* tmp = it->second;
				tmp->g_pTradeUserApi->Release();
				tmp = NULL;
			}
			m_trade.clear();
		}

		hProcess = CreateThread(NULL, 0, MProcess, &instruList, 0, NULL);
		Sleep(1000);
		hThread = CreateThread(NULL, 0, MThread, &instrusMap, 0, NULL);
	}
}

//停止交易
void CCircleTraderMFCDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (hProcess && hThread)
	{
		CloseHandle(hProcess);
		CloseHandle(hThread);
		hProcess = NULL;
		hThread = NULL;
		//重新获取合约，创建交易实例
		market->ReleaseUserApi();
		map<string, CustomTradeSpi*>::iterator it = m_trade.begin();
		for (; it != m_trade.end(); ++it)
		{
			CustomTradeSpi* tmp = it->second;
			tmp->g_pTradeUserApi->Release();
			tmp = NULL;
		}
		m_trade.clear();
		cout << "关闭交易-----------------------" << endl;
		Sleep(1000);
		MessageBox(TEXT("关闭交易成功！"));
	}
}



