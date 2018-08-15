#ifndef _H_CUUSTOMTRADESPI_
#define _H_CUUSTOMTRADESPI_

#include <stdio.h>
#include <map>
#include <string>
#include <ctime>
#include <string.h>
#include <stdlib.h>
#include "head.h"
#include "CTP_API/ThostFtdcTraderApi.h"
#define ZHENGZHOUEXCHANGID "CZCE"
#define SHANGHAIEXCHANGID "SHFE"
#define DALIANEXCHANGID "DCE"
using namespace std;

#define IS_NULL(data) if(data==NULL) return

class CustomTradeSpi : public CThostFtdcTraderSpi
{
public:
    CustomTradeSpi(
            TThostFtdcBrokerIDType gBrokerID,
            TThostFtdcInvestorIDType gInvesterID,
            TThostFtdcPasswordType gInvesterPassword,
            FrontAddress gTradeFrontAddr,
            const TThostFtdcInstrumentIDType g_pTradeInstrumentID,
            Strategy *stra);
    virtual ~CustomTradeSpi();

    void connectTrade();
    void tradeJoin();

    virtual void OnFrontConnected();
    virtual void OnFrontDisconnected(int nReason);
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
    virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
    virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
    virtual void OnRtnTrade(CThostFtdcTradeField *pTrade) ;
    virtual void OnRspQryInvestor(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
    virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
    virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) ;
    virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
    //virtual void OnRspQryProduct(CThostFtdcProductField *pProduct, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
private:
    //持仓 昨买 今买 昨卖 今卖
    CThostFtdcInvestorPositionField buy,sell;
public:
    void getPosition(CThostFtdcInvestorPositionField & buy,
                     CThostFtdcInvestorPositionField& sell);
    void QryAccountMemory();
    void QryInvestorPosition();
    void QryInvestorPositionDetail();
    void OrderAction(char offsetFlag,
                     TThostFtdcDirectionType	Direction,
                     TThostFtdcVolumeType volume,
                     TThostFtdcVolumeConditionType volumeCondition,
                     TThostFtdcVolumeType minVolume,
                     TThostFtdcPriceType price,
                     TThostFtdcTimeConditionType timeCondition,
					 TThostFtdcExchangeIDType ExchangeID,
					 int orderRef,
                     TThostFtdcRequestIDType	RequestID = 0
                     );
    void CancelOrderAction(
            TThostFtdcOrderRefType	OrderRef,
            TThostFtdcExchangeIDType	ExchangeID,
            TThostFtdcOrderSysIDType	OrderSysID
            );
    void QryInstrument();

public:
	CThostFtdcTraderApi * g_pTradeUserApi;                    // 交易指针
private:
    //ctp
    TThostFtdcBrokerIDType gBrokerID;                         // 模拟经纪商代码
    TThostFtdcInvestorIDType gInvesterID;                         // 投资者账户名
    TThostFtdcPasswordType gInvesterPassword;                     // 投资者密码


    FrontAddress gTradeFrontAddr;            // 模拟交易前置地址

    TThostFtdcFrontIDType	FrontID;
    TThostFtdcSessionIDType	SessionID;

    //strategy handle project
    Strategy *strategy;

    map<string,int> order_trade;

public:
    TThostFtdcInstrumentIDType g_pTradeInstrumentID;
    TThostFtdcPriceType PriceTick;
};



#endif
