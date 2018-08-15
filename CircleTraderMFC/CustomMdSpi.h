#ifndef _H_CUSTOMMDSPI_
#define _H_CUSTOMMDSPI_

#include "head.h"
#include "CustomTradeSpi.h"
#include "CTP_API/ThostFtdcMdApi.h"
#include <deque>
#include <map>
#include <string>
#include <string.h>
#include <stdlib.h>
using namespace std;

//class Strategy;

class CustomMdSpi: public CThostFtdcMdSpi  
{  
public:
    CustomMdSpi(
            TThostFtdcBrokerIDType gBrokerID,
            TThostFtdcInvestorIDType gInvesterID,
            TThostFtdcPasswordType gInvesterPassword,
            FrontAddress gMdFrontAddr,
            char**g_pInstrumentID,
            int instrumentNum,
            Strategy *stra);
    ~CustomMdSpi();
private:
    void initMemory();
public:
    void connectMarket();
    void marketJoin();

	// ---- 继承自CTP父类的回调接口并实现 ---- //  
public:  
    void OnFrontConnected();
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);  
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);  
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);  
	
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
public:
	void ReqUserLogout();
	void ReLoginMarket();
	void SetInstruments(char** g_pInstrumentID, int instrumentNum);
	void SetMdUserApi(CThostFtdcMdApi *g_pMdUserApi);
	void ReleaseUserApi();
	CThostFtdcMdApi *g_pMdUserApi;
private:
    //ctp
    TThostFtdcBrokerIDType gBrokerID;                         // 模拟经纪商代码
    TThostFtdcInvestorIDType gInvesterID;                         // 投资者账户名
    TThostFtdcPasswordType gInvesterPassword;                     // 投资者密码

    // 行情参数
                              // 行情指针
    FrontAddress gMdFrontAddr;               // 模拟行情前置地址
    char **g_pInstrumentID; // 行情合约代码列表，中、上、大、郑交易所各选一种
    int instrumentNum;                                             // 行情合约订阅数量

    //instrategy project
    Strategy *strategy;
};  

#endif
