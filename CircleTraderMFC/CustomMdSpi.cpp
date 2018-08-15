#include "stdafx.h"
#include "CustomMdSpi.h"

CustomMdSpi::CustomMdSpi(
        TThostFtdcBrokerIDType gBrokerID,
        TThostFtdcInvestorIDType gInvesterID,
        TThostFtdcPasswordType gInvesterPassword,
        FrontAddress gMdFrontAddr,
        char**g_pInstrumentID,
        int instrumentNum,
        Strategy *stra)
{
    strcpy_s(this->gBrokerID,strlen(gBrokerID)+1,gBrokerID);
    strcpy_s(this->gInvesterID, strlen(gInvesterID)+1,gInvesterID);
    strcpy_s(this->gInvesterPassword, strlen(gInvesterPassword)+1,gInvesterPassword);
    strcpy_s(this->gMdFrontAddr, strlen(gMdFrontAddr)+1,gMdFrontAddr);
    this->g_pInstrumentID=(char **)malloc(sizeof(char*)*instrumentNum);
	memset(this->g_pInstrumentID, 0, sizeof(char*)*instrumentNum);
    for(int i=0;i<instrumentNum;i++)
    {
		
        this->g_pInstrumentID[i]=(char *)malloc(sizeof(g_pInstrumentID[i]));
		memset(this->g_pInstrumentID[i], 0, sizeof(g_pInstrumentID[i]));
        strcpy_s(this->g_pInstrumentID[i], strlen(g_pInstrumentID[i])+1,g_pInstrumentID[i]);
		cout << "constract market " << this->g_pInstrumentID[i] << endl;
    }
    this->instrumentNum=instrumentNum;

    this->strategy=stra;

    g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();   // 创建行情实例

	cout << "construct function mdspi" << endl;
    initMemory();
}
CustomMdSpi::~CustomMdSpi()
{
	//g_pMdUserApi->Release();
	//g_pMdUserApi = NULL;
    for(int i=0;i<instrumentNum;i++)
    {
        free(g_pInstrumentID[i]);
		g_pInstrumentID[i] = NULL;
    }
    free(g_pInstrumentID);
	g_pInstrumentID = NULL;
}
void CustomMdSpi::initMemory()
{
}

void CustomMdSpi::connectMarket()
{
    g_pMdUserApi->RegisterSpi(this);               // 注册事件类
	cout << gMdFrontAddr << endl;
    g_pMdUserApi->RegisterFront(gMdFrontAddr);           // 设置行情前置地址
    g_pMdUserApi->Init();                                // 连接运行
}

void CustomMdSpi::marketJoin()
{
    this->g_pMdUserApi->Join();
    this->g_pMdUserApi->Release();
}


// 连接成功应答  
void CustomMdSpi::OnFrontConnected()  
{  
	cout << "=====connect network success mdspi=====" << endl;  
	// 开始登录  
	CThostFtdcReqUserLoginField loginReq;  
	memset(&loginReq, 0, sizeof(loginReq));  
	strcpy_s(loginReq.BrokerID,strlen(gBrokerID)+1, gBrokerID);  
	strcpy_s(loginReq.UserID, strlen(gInvesterID) + 1, gInvesterID);
	strcpy_s(loginReq.Password, strlen(gInvesterPassword) + 1, gInvesterPassword);

	static int requestID = 0; // 请求编号  
	int rt = g_pMdUserApi->ReqUserLogin(&loginReq, requestID);  
	if (!rt)  
		cout << ">>>>>>send login request success" << endl;  
	else  
		cerr << "--->>>send login request fail" << endl;  
}  

// 登录应答  
void CustomMdSpi::OnRspUserLogin(  
	CThostFtdcRspUserLoginField *pRspUserLogin,   
	CThostFtdcRspInfoField *pRspInfo,   
	int nRequestID,   
	bool bIsLast)  
{  
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);  
	if (!bResult)  
	{  
		cout << "=====account login success=====" << endl;  
		cout << "trade day： " << pRspUserLogin->TradingDay << endl;  
		cout << "login time： " << pRspUserLogin->LoginTime << endl;  
		cout << "broker id： " << pRspUserLogin->BrokerID << endl;  
		cout << "account name： " << pRspUserLogin->UserID << endl;  
		for (int i = 0; i < instrumentNum; ++i)
		{
			cout << g_pInstrumentID[i] << endl;
		}
		
		// 开始订阅行情  
		int rt = g_pMdUserApi->SubscribeMarketData(g_pInstrumentID, instrumentNum);  
		if (!rt)  
			cout << ">>>>>>send market request success" << endl;  
		else  
			cerr << "--->>>send market request fail" << endl;  
	}  
	else  
		cerr << "reponse error--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;  
}

// 订阅行情应答  
void CustomMdSpi::OnRspSubMarketData(  
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,   
	CThostFtdcRspInfoField *pRspInfo,   
	int nRequestID,   
	bool bIsLast)  
{  
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);  
	if (!bResult)  
	{  
		cout << "=====login market success=====" << endl;  
		cout << "instruID ： " << pSpecificInstrument->InstrumentID << endl;  
	}  
	else  
		cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;  
}

// 行情详情通知  
void CustomMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)  
{  
    if(pDepthMarketData==NULL)
        return ;
	if (this->strategy != NULL)
	{
		//cout << "================new market " << endl;
		this->strategy->marketMessage(pDepthMarketData);
		//cout << "================new market " << endl;
	}
        
    return ;

}

void CustomMdSpi::OnRspUserLogout(CThostFtdcUserLogoutField * pUserLogout, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	//for (int i = 0; i<instrumentNum; i++)
	//{
	//	free(g_pInstrumentID[i]);
	//	g_pInstrumentID[i] = NULL;
	//}
	//free(g_pInstrumentID);
	//g_pInstrumentID = NULL;
}

void CustomMdSpi::ReqUserLogout()
{
	cout << "=====ReLogin Market=====" << endl;
	static int requestID = 0; // 请求编号  
	CThostFtdcUserLogoutField logoutReq;
	memset(&logoutReq, 0, sizeof(logoutReq));
	strcpy_s(logoutReq.BrokerID, strlen(gBrokerID) + 1, gBrokerID);
	strcpy_s(logoutReq.UserID, strlen(gInvesterID) + 1, gInvesterID);
	int ret = g_pMdUserApi->ReqUserLogout(&logoutReq, 0);
	if (!ret)
	{
		cout << ">>>>>>send logout request success" << endl;
	}
	else
	{
		cout << ">>>>>>send logout request success" << endl;
	}
}

void CustomMdSpi::ReLoginMarket()
{
	static int requestID = 0; // 请求编号  
	// 开始登录  

	CThostFtdcReqUserLoginField loginReq;
	memset(&loginReq, 0, sizeof(loginReq));
	strcpy_s(loginReq.BrokerID, strlen(gBrokerID) + 1, gBrokerID);
	strcpy_s(loginReq.UserID, strlen(gInvesterID) + 1, gInvesterID);
	strcpy_s(loginReq.Password, strlen(gInvesterPassword) + 1, gInvesterPassword);
	
	int rt = g_pMdUserApi->ReqUserLogin(&loginReq, requestID);
	if (!rt)
		cout << ">>>>>>send login request success" << endl;
	else
		cerr << "--->>>send login request fail" << endl;
}

void CustomMdSpi::SetInstruments(char** g_pInstrumentID, int instrumentNum)
{
	this->g_pInstrumentID = (char **)malloc(sizeof(char*)*instrumentNum);
	memset(this->g_pInstrumentID, 0, sizeof(char*)*instrumentNum);
	for (int i = 0; i<instrumentNum; i++)
	{

		this->g_pInstrumentID[i] = (char *)malloc(sizeof(g_pInstrumentID[i]));
		memset(this->g_pInstrumentID[i], 0, sizeof(char *));
		strcpy_s(this->g_pInstrumentID[i], strlen(g_pInstrumentID[i]) + 1, g_pInstrumentID[i]);
		cout << "constract market " << this->g_pInstrumentID[i] << endl;
	}
	cout << "g_pInstrumentID[1]: " << this->g_pInstrumentID[1] << endl;
	this->instrumentNum = instrumentNum;

}

void CustomMdSpi::SetMdUserApi(CThostFtdcMdApi * g_pMdUserApi)
{
	this->g_pMdUserApi = g_pMdUserApi;
}

void CustomMdSpi::ReleaseUserApi()
{
	g_pMdUserApi->Release();
	g_pMdUserApi = NULL;
}

